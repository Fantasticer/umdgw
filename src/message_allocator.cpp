#include "message_allocator.hpp"

#include <stdlib.h>

namespace umdgw {

  message_allocator_t::message_allocator_t()
  : enable_recycle_(false) {
  memset(free_lists_, 0, sizeof(free_lists_));
}

message_allocator_t::~message_allocator_t() {
  ConsumeIdleQueue();

  int size = sizeof(free_lists_)/sizeof(message_t*);
  for (int i = 0; i < size; ++i) {
    message_t* free_head = free_lists_[i];
    while(nullptr != free_head) {
      message_t* current = free_head;
      free_head = free_head->next();
      delete current;
    }
  }
}

int message_allocator_t::Init(bool enable_recycle) {
  enable_recycle_ = enable_recycle;
  return idle_queue_.Init();
}

int message_allocator_t::Allocate(int capacity, message_t** ret_msg) {
  message_t* result = nullptr;
  // too big, do not cache
  if (capacity > (1 << kAllocateStop)) {
    result = new (std::nothrow) message_t();
    result->Init(-1, capacity);
    *ret_msg = result;
    return 0;
  }

  // find log base 2
  uint32_t v = (uint32_t) capacity;
  static const int MultiplyDeBruijnBitPosition[32] = {
    0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
    8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
  };
  // first round down to one less than a power of 2
  v |= v >> 1; 
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;

  int index = MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
  index = index > kAllocateStart ? index - kAllocateStart : 0;

  message_t* free_head = free_lists_[index];
  if (nullptr == free_head) {
    ConsumeIdleQueue();
    free_head = free_lists_[index];
  }

  if (free_head) {
    result = free_head;
    free_head = free_head->next();
    result->set_next(nullptr);
    free_lists_[index] = free_head;
  } else {
    result = new (std::nothrow) message_t();
    result->Init(index, 1 << (kAllocateStart + index + 1));
  }
  *ret_msg = result;
  return 0;
}

void message_allocator_t::Release(message_t* msg) {
  int index = msg->index();
  if (index < 0) {
    delete msg;
    return;
  }

  message_t* free_head = free_lists_[index];
  msg->Clear();
  msg->set_next(free_head);
  free_lists_[index] = msg;
}

int message_allocator_t::Recycle(message_t* msg) {
  int index = msg->index();
  if (index < 0) {
    delete msg;
    return 0;
  }
  idle_queue_.Write(msg, false);
  idle_queue_.Flush();
  return 0;
}

void message_allocator_t::ConsumeIdleQueue() {
  if (!enable_recycle_) {
    return;
  }

  message_t* msg = nullptr;
  while (idle_queue_.Read(&msg)) {
    int index = msg->index();
    message_t* free_head = free_lists_[index];
    msg->set_next(free_head);
    free_lists_[index] = msg;
  }
}

} // namespace vss