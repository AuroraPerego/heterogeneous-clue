#include <limits>

#include "CUDACore/allocate_host.h"
#include "CUDACore/cudaCheck.h"

#include "getCachingHostAllocator.h"

namespace {
  const size_t maxAllocationSize =
      notcub::CachingDeviceAllocator::IntPow(cms::hip::allocator::binGrowth, cms::hip::allocator::maxBin);
}

namespace cms::hip {
  void *allocate_host(size_t nbytes, hipStream_t stream) {
    void *ptr = nullptr;
    if constexpr (allocator::useCaching) {
      if (nbytes > maxAllocationSize) {
        throw std::runtime_error("Tried to allocate " + std::to_string(nbytes) +
                                 " bytes, but the allocator maximum is " + std::to_string(maxAllocationSize));
      }
      cudaCheck(allocator::getCachingHostAllocator().HostAllocate(&ptr, nbytes, stream));
    } else {
      cudaCheck(hipHostMalloc(&ptr, nbytes));
    }
    return ptr;
  }

  void free_host(void *ptr) {
    if constexpr (allocator::useCaching) {
      cudaCheck(allocator::getCachingHostAllocator().HostFree(ptr));
    } else {
      cudaCheck(hipHostFree(ptr));
    }
  }

}  // namespace cms::hip
