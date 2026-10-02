#pragma once

#include <cstddef>
#include <cstdlib>
#include <map>
#include <mutex>
#include <new>
#include <string>

namespace allocation_tracker {

// Global statistics for every new/delete pair (sized-delete aware).
// Usable as: std::set_new_handler(allocation_tracker::onAllocationFailure)
// and as a counter of outstanding allocations.
struct Stats {
    std::size_t totalAllocations = 0;
    std::size_t totalDeallocations = 0;
    std::size_t liveAllocations = 0;
    std::size_t liveBytes = 0;
    std::size_t peakBytes = 0;
    std::size_t failedAllocations = 0;
};

// Thread-safe access to process-wide counters.
Stats snapshot();
void reset();
void recordAllocation(std::size_t bytes);
void recordDeallocation(std::size_t bytes);
void recordFailure();

// RAII scoped counter: everything allocated inside the scope is attributed.
// (Does NOT replace global new - it wraps a simple counting allocator
// for containers/objects you choose to track.)
template <typename T>
class CountingAllocator {
public:
    using value_type = T;

    CountingAllocator() noexcept = default;
    template <typename U>
    CountingAllocator(const CountingAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        recordAllocation(n * sizeof(T));
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }
    void deallocate(T* p, std::size_t n) noexcept {
        if (p) {
            recordDeallocation(n * sizeof(T));
            std::free(p);
        }
    }

    template <typename U>
    bool operator==(const CountingAllocator<U>&) const noexcept { return true; }
    template <typename U>
    bool operator!=(const CountingAllocator<U>&) const noexcept { return false; }
};

// The new_handler: called by the runtime when an allocation fails.
void onAllocationFailure();

}  // namespace allocation_tracker
