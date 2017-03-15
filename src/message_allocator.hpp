
#ifndef __UMDGW_MESSAGE_ALLOCATOR_HPP_INCLUDED__
#define __UMDGW_MESSAGE_ALLOCATOR_HPP_INCLUDED__

#include "message.hpp"
#include "spsc_queue.hpp"
#include<new>
namespace umdgw {

/// The class to allocate messages, A internal message_t pool is implemented in
/// this class, when allocating a message_t, this class would map the size to 
/// corresponding free list to see whether a free message_t is available.
class message_allocator_t {
 public:
  friend class pipe_t;    // for access Recycle

  /// Creates a message_t allocator
  message_allocator_t();
  /// Destroy the message_t allocator, release all memory allocated by
  /// this allocator
  ~message_allocator_t();

  /// Initialize the allocator
  int Init(bool enable_recycle);

  /// Allocate a message_t with the specified capacity
  int Allocate(int capacity, message_t** ret_msg);

  /// Releases the specified message_t, just to add back to the corresponding
  /// free list
  void Release(message_t* msg);

 private:
  const static int kAllocateStart = 5;
  const static int kAllocateStop = 22;
  // Number of new message_ts in idle message_t pipe needed to trigger new memory
  // allocation.
  const static int kIdlemessage_tGranularity = 256;

  /// Recycle the specified message_t. The difference with the Release message_t
  /// is that, this method try to put the message_t to a idle queue, this queue
  /// is lock free, so the Recycle method is intended to be called by another
  /// thread to release the message_t.
  int Recycle(message_t* msg);

  // Helper method to release methods in the idle queue
  void ConsumeIdleQueue();

  // Whether recycle functionality is enabled
  bool enable_recycle_;
  // An array of free message_t lists. Each free list has the same capacity of
  // memory allocated, and the size is the 2 ^ (kAllocateStart + array_index)
  message_t* free_lists_[kAllocateStop - kAllocateStart + 1];

  // The idle message_ts queue that are released by the back thread
  SpscQueue<message_t*, kIdlemessage_tGranularity> idle_queue_;

};

}

#endif // __UMDGW_MESSAGE_ALLOCATOR_HPP_INCLUDED__