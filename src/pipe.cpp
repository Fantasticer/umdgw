
#include "pipe.hpp"
#include "message_allocator.hpp"

namespace umdgw {

  int CreatePipePair(boost::shared_ptr<pipe_t> pipes[2], int index) {
    boost::shared_ptr<pipe_t> pipe0(new (std::nothrow) pipe_t(index));
    boost::shared_ptr<pipe_t> pipe1(new (std::nothrow) pipe_t(index));

    pipe_t::messageQueue* queues[2] = { 0 };
    queues[0] = new (std::nothrow) pipe_t::messageQueue();
    queues[1] = new (std::nothrow) pipe_t::messageQueue();
    queues[0]->Init();
    queues[1]->Init();

    pipe0->Init(queues[0], queues[1], pipe1);
    pipe1->Init(queues[1], queues[0], pipe0);
    pipes[0] = pipe0;
    pipes[1] = pipe1;
    return 0;
  }

  pipe_t::pipe_t(int index)
    : listener_(nullptr)
    , terminated_(false)
    , peer_terminated_(false)
    , allocator_(nullptr)
    , in_queue_(nullptr)
    , out_queue_(nullptr)
    , index_(index) {
    // nothing
  }

  pipe_t::~pipe_t() {
    if (nullptr != listener_) {
      listener_->OnDestroy(this);
    }
    // Only destroy the in queue, the out queue would be destroyed
    // by the peer pipe object
    if (nullptr != in_queue_) {
      delete in_queue_;
      in_queue_ = nullptr;
    }
    if (nullptr != allocator_) {
      delete allocator_;
      allocator_ = nullptr;
    }
  }

  int pipe_t::Init(messageQueue* in_queue, messageQueue* out_queue,
    boost::shared_ptr<pipe_t> peer) {
    allocator_ = new (std::nothrow) message_allocator_t();
    allocator_->Init(true);
    peer_ = peer;
    in_queue_ = in_queue;
    out_queue_ = out_queue;
    return 0;
  }

  int pipe_t::Write(message_t* msg, bool incomplete) {
    if (terminated_) {
      return -1;
    }
    out_queue_->Write(msg, incomplete);
    if (!incomplete) {
      if (!out_queue_->Flush()) {
        // activate the peer pipe reader
        peer_->SendReadActivated();
      }
    }
    return 0;
  }

  int pipe_t::Write(message_t* msg, bool* o_wake_peer) {
    if (terminated_) {
      return -1;
    }
    out_queue_->Write(msg, false);
    *o_wake_peer = false;
    if (!out_queue_->Flush()) {
      *o_wake_peer = true;
      // activate the peer pipe reader
      peer_->SendReadActivated();
    }
    return 0;
  }

  int pipe_t::Unwrite(message_t** res_msg) {
    return out_queue_->Unwrite(res_msg) ? 0 : -1;
  }

  bool pipe_t::CheckRead() {
    if (peer_terminated_) {
      return false;
    }
    return in_queue_->CheckRead();
  }

  int pipe_t::Read(int timeout, message_t** res_msg) {
    int res = 0;
    if (DoRead(res_msg, &res)) {
      return res;
    }
    if (0 == timeout) {
      // do not wait
      return -1;
    }
    { // lock context
      boost::mutex::scoped_lock lock(mutex_);
      if (DoRead(res_msg, &res)) {
        return res;
      }
      if (timeout > 0) {
        condition_.timed_wait(lock, boost::posix_time::millisec(timeout));
      }
      else {
        // wait forever
        condition_.wait(lock);
      }
      if (timeout > 0) {
        if (DoRead(res_msg, &res)) {
          return res;
        }
      }
      else {
        while (true) {
          if (DoRead(res_msg, &res)) {
            return res;
          }
          // wait forever
          condition_.wait(lock);
        }
      }
    }
    return -1;
  }

  int pipe_t::Terminate() {
    if (terminated_) {
      return 0;
    }
    out_queue_->Write(nullptr, false);
    if (!out_queue_->Flush()) {
      // activate the peer pipe reader
      peer_->SendReadActivated();
    }
    terminated_ = true;
    // release reference to the peer as we would not need to write anymore
    peer_.reset();
    return 0;
  }

  void pipe_t::Releasemessage_t(message_t* msg) {
    allocator_->Release(msg);
  }

  void pipe_t::Recyclemessage_t(message_t* msg) {
    peer_->allocator_->Recycle(msg);
  }

  void pipe_t::SendReadActivated() {
    if (nullptr != listener_) {
      listener_->OnReadActivated(this);
    }
    { // lock context
      boost::mutex::scoped_lock lock(mutex_);
      condition_.notify_all();
    }
  }

  bool pipe_t::DoRead(message_t** res_msg, int* res) {
    if (peer_terminated_) {
      *res = -1;
      return true;
    }
    if (in_queue_->Read(res_msg)) {
      if (nullptr == *res_msg) {
        peer_terminated_ = true;
        *res = -1;
        return true;
      }
      *res = 0;
      return true;
    }
    return false;
  }

} // namespace umdgw
