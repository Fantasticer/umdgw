#ifndef __UMDGW_MESSAGE_HPP_INCLUDED__
#define __UMDGW_MESSAGE_HPP_INCLUDED__

#include<stdint.h>
#include<malloc.h>
#include<string>

namespace umdgw {

  class message_t {
  public:
    message_t()
      : index_(0)
      , capacity_(0)
      , size_(0)
      , buffer_(nullptr)
      , timestamp_(0)
      , id_(0) {
      // nothing
    }
    ~message_t() {
      if (nullptr != buffer_) {
        free(buffer_);
        buffer_ = nullptr;
      }
    }

    /// Initializes the message_t with the index of the corresponding free
    /// list. May fail when memory allocation failed.
    int Init(int index, int capacity) {
      index_ = index;
      capacity_ = capacity;
      size_ = 0;
      timestamp_ = 0;
      id_ = 0;
      buffer_ = reinterpret_cast<unsigned char*>(malloc(capacity_));
      return 0;
    }
    /// Copy the content of the message_t to this message_t
    void Copy(const message_t* src) {
      size_ = src->size_;
      memcpy_s(buffer_, capacity_, src->buffer_, src->size_);
      timestamp_ = src->timestamp_;
      id_ = src->id_;
      next_ = nullptr;
    }

    void Clear() {
      size_ = 0;
      next_ = nullptr;
      timestamp_ = 0;
      id_ = 0;
    }

    int index() const {
      return index_;
    }

    uint8_t* buffer() {
      return buffer_;
    }

    const uint8_t* buffer() const {
      return buffer_;
    }

    int capacity() const {
      return capacity_;
    }

    int size() const {
      return size_;
    }

    void set_size(int size) {
      size_ = size;
    }

    int id() const {
      return id_;
    }

    void set_id(int id) {
      id_ = id;
    }

    int64_t timestamp() const {
      return timestamp_;
    }

    void set_timestamp(int64_t timestamp) {
      timestamp_ = timestamp;
    }

    message_t* next() const {
      return next_;
    }

    void set_next(message_t* next) {
      next_ = next;
    }

  private:
    int index_;
    int capacity_;
    // size of the message_t
    int size_;
    // the id of the message_t
    int id_;
    // the timestamp
    int64_t timestamp_;
    // buffer to store the message_t content
    uint8_t* buffer_;
    // link to the next message_t in the free lists
    message_t* next_;
  };

}


#endif // !__UMDGW_MESSAGE_HPP_INCLUDED__

