
#ifndef __UMDGW_SPSC_QUEUE_HPP_INCLUDED__
#define __UMDGW_SPSC_QUEUE_HPP_INCLUDED__

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include <boost/atomic.hpp>

namespace umdgw {

//  This class is an efficient queue implementation. The main goal is
//  to minimize number of allocations/deallocations needed. Thus it
//  allocates/deallocates elements in batches of N.
//
//  This class allows one thread to use push/back function and another one 
//  to use pop/front functions. However, user must ensure that there's no
//  pop on the empty queue and that both threads don't access the same
//  element in unsynchronized manner.
//
//  T is the type of the object in the queue.
//  N is granularity of the queue (how many pushes have to be done till
//  actual memory allocation is required).
template <typename T, int N> 
class BatchQueue {
 public:
  // Create the queue.
  inline BatchQueue () 
    : begin_chunk_(nullptr)
    , end_chunk_(nullptr)
    , back_chunk_(nullptr)
    , begin_pos_(0)
    , back_pos_(0)
    , end_pos_(0)
    , spare_chunk_(nullptr) {}
  // Destroy the queue.
  inline ~BatchQueue () {
    while (true) {
      if (begin_chunk_ == end_chunk_) {
        free(begin_chunk_);
        break;
      } 
      Chunk *o = begin_chunk_;
      begin_chunk_ = begin_chunk_->next;
      free (o);
    }

    Chunk *sc = spare_chunk_.exchange(nullptr, boost::memory_order_relaxed);
    free (sc);
  }

  /// Initialize the queue. May fail when memory allocation failed.
  inline int Init() {
    begin_chunk_ = (Chunk*) malloc (sizeof (Chunk));
    if (nullptr == begin_chunk_) return -1; 
    begin_pos_ = 0;
    back_chunk_ = nullptr;
    back_pos_ = 0;
    end_chunk_ = begin_chunk_;
    end_pos_ = 0;
    return 0;
  }

  // Returns reference to the front element of the queue.
  // If the queue is empty, behavior is undefined.
  inline T& Front() {
    return begin_chunk_->values[begin_pos_];
  }

  //  Returns reference to the back element of the queue.
  //  If the queue is empty, behavior is undefined.
  inline T& Back() {
    return back_chunk_->values[back_pos_];
  }

  /// Adds an element to the back end of the queue.
  /// May fail when memory allocation failed.
  inline int Push() {
    back_chunk_ = end_chunk_;
    back_pos_ = end_pos_;

    if (++end_pos_ != N)
      return 0;

    Chunk* sc = spare_chunk_.exchange(nullptr, boost::memory_order_relaxed);
    if (sc) {
      end_chunk_->next = sc;
      sc->prev = end_chunk_;
    } else {
      Chunk* new_chunk = (Chunk*)malloc(sizeof(Chunk));
      if (nullptr == new_chunk) {
        --end_pos_;
        return -1;
      }
      end_chunk_->next = new_chunk;
      end_chunk_->next->prev = end_chunk_;
    }
    end_chunk_ = end_chunk_->next;
    end_pos_ = 0;
    return 0;
  }

  //  Removes element from the back end of the queue. In other words
  //  it rollbacks last push to the queue. Take care: Caller is
  //  responsible for destroying the object being unpushed.
  //  The caller must also guarantee that the queue isn't empty when
  //  unpush is called. It cannot be done automatically as the read
  //  side of the queue can be managed by different, completely
  //  unsynchronized thread.
  inline void Unpush() {
    //  First, move 'back' one position backwards.
    if (back_pos_) {
      --back_pos_;
    } else {
      back_pos_ = N - 1;
      back_chunk_ = back_chunk_->prev;
    }

    //  Now, move 'end' position backwards. Note that obsolete end chunk
    //  is not used as a spare chunk. The analysis shows that doing so
    //  would require free and atomic operation per chunk deallocated
    //  instead of a simple free.
    if (end_pos_) {
      --end_pos_;
    } else {
      end_pos_ = N - 1;
      end_chunk_ = end_chunk_->prev;
      free (end_chunk_->next);
      end_chunk_->next = nullptr;
    }
  }

  //  Removes an element from the front end of the queue.
  inline void Pop () {
    if (++ begin_pos_ == N) {
      Chunk* o = begin_chunk_;
      begin_chunk_ = begin_chunk_->next;
      begin_chunk_->prev = nullptr;
      begin_pos_ = 0;

      //  'o' has been more recently used than spare_chunk,
      //  so for cache reasons we'll get rid of the spare and
      //  use 'o' as the spare.
      Chunk* cs = spare_chunk_.exchange(o, boost::memory_order_relaxed);
      free(cs);
    }
  }

 private:

  //  Individual memory chunk to hold N elements.
  struct Chunk {
    T values [N];
    Chunk* prev;
    Chunk* next;
  };

  //  Back position may point to invalid memory if the queue is empty,
  //  while begin & end positions are always valid. Begin position is
  //  accessed exclusively be queue reader (front/pop), while back and
  //  end positions are accessed exclusively by queue writer (back/push).
  Chunk* begin_chunk_;
  int begin_pos_;
  Chunk* back_chunk_;
  int back_pos_;
  Chunk* end_chunk_;
  int end_pos_;

  //  People are likely to produce and consume at similar rates.  In
  //  this scenario holding onto the most recently freed chunk saves
  //  us from having to call malloc/free.
  boost::atomic<Chunk*> spare_chunk_;
};

///  Lock-free queue implementation.
///  Only a single thread can read from the pipe at any specific moment.
///  Only a single thread can write to the pipe at any specific moment.
///  T is the type of the object in the queue.
///  N is granularity of the pipe, i.e. how many items are needed to
///  perform next memory allocation.
template <typename T, int N> 
class SpscQueue {
 public:

  //  Initializes the queue.
  inline SpscQueue() 
    : r_item_(nullptr), w_item_(nullptr), f_item_(nullptr), contention_(nullptr) {}

  //  The destructor doesn't have to be virtual. It is made virtual
  //  just to keep ICC and code checking tools from complaining.
  inline virtual ~SpscQueue() {}

  /// Initialize the queue. May fail when memory allocation failed.
  inline int Init() {
    queue_.Init();
    //  Insert terminator element into the queue.
    queue_.Push();

    //  Let all the pointers to point to the terminator.
    //  (unless pipe is dead, in which case c is set to nullptr).
    r_item_ = w_item_ = f_item_ = &queue_.Back ();
    contention_.store(&queue_.Back ());
    return 0;
  }

  //  Write an item to the pipe.  Don't flush it yet. If incomplete is
  //  set to true the item is assumed to be continued by items
  //  subsequently written to the pipe. Incomplete items are never
  //  flushed down the stream.
  // May fail on memory allocation
  inline int Write(const T& value, bool incomplete) {
    //  Place the value to the queue, add new terminator element.
    queue_.Back () = value;
    queue_.Push();

    //  Move the "flush up to here" pointer.
    if (!incomplete) {
      f_item_ = &queue_.Back ();
    }
    return 0;
  }
  //  Pop an incomplete item from the pipe. Returns true is such
  //  item exists, false otherwise.
  inline bool Unwrite(T* value) {
    if (f_item_ == &queue_.Back ()) {
      return false;
    }
    queue_.Unpush ();
    *value = queue_.Back ();
    return true;
  }

  //  Flush all the completed items into the pipe. Returns false if
  //  the reader thread is sleeping. In that case, caller is obliged to
  //  wake the reader up before using the pipe again.
  inline bool Flush() {
    //  If there are no un-flushed items, do nothing.
    if (w_item_ == f_item_) {
      return true;
    }

    //  Try to set 'c' to 'f'.
    if (!contention_.compare_exchange_strong(w_item_, f_item_, 
      boost::memory_order_acq_rel)) {
      //  Compare-and-swap was unsuccessful because 'c' is nullptr.
      //  This means that the reader is asleep. Therefore we don't
      //  care about thread-safeness and update c in non-atomic
      //  manner. We'll return false to let the caller know
      //  that reader is sleeping.
      contention_.store(f_item_, boost::memory_order_relaxed);
      w_item_ = f_item_;
      return false;
    }

    //  Reader is alive. Nothing special to do now. Just move
    //  the 'first un-flushed item' pointer to 'f'.
    w_item_ = f_item_;
    return true;
  }

  //  Check whether item is available for reading.
  inline bool CheckRead() {
    //  Was the value prefetched already? If so, return.
    if (&queue_.Front () != r_item_ && r_item_) {
      return true;
    }

    //  There's no prefetched value, so let us prefetch more values.
    //  Prefetching is to simply retrieve the
    //  pointer from c in atomic fashion. If there are no
    //  items to prefetch, set c to nullptr (using compare-and-swap).
    T* first_item = &queue_.Front();
    contention_.compare_exchange_strong(first_item, nullptr, 
      boost::memory_order_acq_rel);
    r_item_ = first_item;

    //  If there are no elements prefetched, exit.
    //  During pipe's lifetime r should never be nullptr, however,
    //  it can happen during pipe shutdown when items
    //  are being deallocated.
    if (&queue_.Front () == r_item_ || !r_item_) {
      return false;
    }

    //  There was at least one value prefetched.
    return true;
  }

  //  Reads an item from the pipe. Returns false if there is no value.
  //  available.
  inline bool Read(T* value) {
    //  Try to prefetch a value.
    if (!CheckRead ())
      return false;

    //  There was at least one value prefetched.
    //  Return it to the caller.
    *value = queue_.Front ();
    queue_.Pop ();
    return true;
  }

  //  Applies the function fn to the first element in the pipe
  //  and returns the value returned by the fn.
  //  The pipe mustn't be empty or the function crashes.
  inline bool Probe (bool (*fn)(T &)) {
    bool rc = CheckRead ();
    if (!rc) return rc;

    return (*fn) (queue_.Front ());
  }

 protected:

  //  Allocation-efficient queue to store pipe items.
  //  Front of the queue points to the first prefetched item, back of
  //  the pipe points to last un-flushed item. Front is used only by
  //  reader thread, while back is used only by writer thread.
  BatchQueue<T, N> queue_;

  //  Points to the first un-flushed item. This variable is used
  //  exclusively by writer thread.
  T* w_item_;

  //  Points to the first un-prefetched item. This variable is used
  //  exclusively by reader thread.
  T* r_item_;

  //  Points to the first item to be flushed in the future.
  T* f_item_;

  //  The single point of contention between writer and reader thread.
  //  Points past the last flushed item. If it is nullptr,
  //  reader is asleep. This pointer should be always accessed using
  //  atomic operations.
  boost::atomic<T*> contention_;

};

} // namespace umdgw

#endif // __UMDGW_SPSC_QUEUE_HPP_INCLUDED__