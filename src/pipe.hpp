#ifndef __UMDGW_PIPE_HPP_INCLUDED__
#define __UMDGW_PIPE_HPP_INCLUDED__

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "spsc_queue.hpp"


namespace umdgw {

    class pipe_t;             // Declare Pipe
    class message_t;          // message_t.h
    class message_allocator_t; // message_t_allocator.h

                            /// The listener for the pipe events
    class PipeEventListener {
    public:
      // Default empty virtual destructor
      virtual ~PipeEventListener() {}
      // Get called when new message_t arrived on the corresponding pipe object
      // so that it changed the state to read activated
      virtual void OnReadActivated(pipe_t* /*pipe*/) = 0;
      // Get called when this pipe is about to be destroyed
      virtual void OnDestroy(pipe_t* /*pipe*/) = 0;
    };

    // The function to create a pair of pipe objects. The two created pipe
    // objects are intended to be operated on two threads, and when write
    // message_t on any of the two pipes, the other pipe would read this message_t
    int CreatePipePair(boost::shared_ptr<pipe_t> pipes[2],int index);

    /// The pipe class used to exchange message_ts between two threads. This
    /// class is only intended to be created through the CreatePipePair
    /// function
    class pipe_t : public boost::enable_shared_from_this<pipe_t> {
    public:
      //  This allows CreatePipePair to create Pipe objects.
      friend int CreatePipePair(boost::shared_ptr<pipe_t> pipes[2], int index);

      // Destroy the pipe
      ~pipe_t();

      // Writes message_t to the pipe
      int Write(message_t* msg, bool incomplete = false);

      // writes message_t to the pipe and return whether we need to awake the peer
      int Write(message_t* msg, bool* o_wake_peer);

      //  Pop an incomplete message_t from the pipe.
      int Unwrite(message_t** res_msg);

      //  Check whether item is available for reading.
      bool CheckRead();

      // Reads message_t from the pipe
      int Read(int timeout, message_t** res_msg);

      // Ask pipe to terminate. 
      int Terminate();

      // Release message_t in case the built message_t is not written
      // to the pipe
      void Releasemessage_t(message_t* msg);
      // The API to recycle message_ts. Each message_t returned from
      // the Read API should be recycled through this API
      void Recyclemessage_t(message_t* msg);

      // Sets the listener
      void set_listener(PipeEventListener* listener) {
        // could only be set once
        listener_ = listener;
      }
      // The message_t allocator.
      message_allocator_t* allocator() const {
        return allocator_;
      }
      int index() const {
        return index_;
      }

    private:
      // Define the message_t queue
      typedef SpscQueue<message_t*, 256> messageQueue;

      //  Constructor is private. pipe can only be created using
      //  PipePair function.
      explicit pipe_t(int index);

      int Init(messageQueue* in_queue, messageQueue* out_queue,
        boost::shared_ptr<pipe_t> peer);

      // Notify this pipe new message_t arrived
      void SendReadActivated();

      bool DoRead(message_t** res_msg, int* res);

      // index of this pipe pair
      int index_;
      PipeEventListener* listener_;
      // The flag indicating whether this pipe has been terminated
      bool terminated_;
      bool peer_terminated_;
      // Hold reference to the peer object so that the peer
      // object would not be destroyed when calling SendReadActivated
      // on it
      boost::shared_ptr<pipe_t> peer_;
      // The message allocator to allocate messages
      message_allocator_t* allocator_;
      // The input queue
      messageQueue* in_queue_;
      // The output queue
      messageQueue* out_queue_;
      // mutex for read waiting
      boost::mutex mutex_;
      // condition for read waiting
      boost::condition_variable condition_;
    };
 }

#endif // !__UMDGW_PIPE_HPP_INCLUDED__

