#ifndef __UMDGW_MEMORY_ALLOCATOR_H_INCLUDED__
#define __UMDGW_MEMORY_ALLOCATOR_H_INCLUDED__
#include<string>
namespace umdgw {

/// This is a helper class used to allocate memory, it may increase memory 
/// size by 50% at least at a time
class MemoryAllocator {
 public:
  MemoryAllocator() 
    : capacity_(0)
    , memory_(nullptr) {}
  ~MemoryAllocator() {
    if (memory_) {
      free(memory_);
      memory_ = nullptr;
    }
  }

  /// Ensures the memory with the specified size allocated and return
  /// the allocated memory
  void* Ensure(int capacity) {
    if (nullptr == memory_) {
      memory_ = malloc(capacity);
      capacity_ = capacity;
      memset(memory_, 0, capacity_);
      return memory_;
    }
    if (capacity > capacity_) {
      int size = capacity_ * 3 / 2;
      int old_capacity = capacity_;
      capacity_ = capacity > size ? capacity : size;
      memory_ = realloc(memory_, capacity_);
      memset(reinterpret_cast<uint8_t*>(memory_) + old_capacity, 0, 
        capacity_ - old_capacity);
    }
    return memory_;
  }

  uint8_t* memory() {
    return reinterpret_cast<uint8_t*>(memory_);
  }

  const uint8_t* memory() const {
    return reinterpret_cast<uint8_t*>(memory_);
  }

 private:
  void* memory_;
  int capacity_;

};

} // namespace umdgw

#endif // __UMDGW_MEMORY_ALLOCATOR_H_INCLUDED__