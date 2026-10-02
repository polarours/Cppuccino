#include "allocation_tracker.hpp"

#include <atomic>

namespace allocation_tracker {

namespace {

std::atomic<std::size_t> gTotalAlloc{0};
std::atomic<std::size_t> gTotalDealloc{0};
std::atomic<std::size_t> gLiveAlloc{0};
std::atomic<std::size_t> gLiveBytes{0};
std::atomic<std::size_t> gPeakBytes{0};
std::atomic<std::size_t> gFailures{0};

}  // namespace

void recordAllocation(std::size_t bytes) {
    gTotalAlloc.fetch_add(1, std::memory_order_relaxed);
    gLiveAlloc.fetch_add(1, std::memory_order_relaxed);
    std::size_t live = gLiveBytes.fetch_add(bytes, std::memory_order_relaxed) + bytes;
    // update peak (racy but monotonic-safe enough for reporting)
    std::size_t peak = gPeakBytes.load(std::memory_order_relaxed);
    while (live > peak &&
           !gPeakBytes.compare_exchange_weak(peak, live,
                                             std::memory_order_relaxed)) {
    }
}

void recordDeallocation(std::size_t bytes) {
    gTotalDealloc.fetch_add(1, std::memory_order_relaxed);
    gLiveAlloc.fetch_sub(1, std::memory_order_relaxed);
    gLiveBytes.fetch_sub(bytes, std::memory_order_relaxed);
}

void recordFailure() {
    gFailures.fetch_add(1, std::memory_order_relaxed);
}

Stats snapshot() {
    Stats s;
    s.totalAllocations = gTotalAlloc.load(std::memory_order_relaxed);
    s.totalDeallocations = gTotalDealloc.load(std::memory_order_relaxed);
    s.liveAllocations = gLiveAlloc.load(std::memory_order_relaxed);
    s.liveBytes = gLiveBytes.load(std::memory_order_relaxed);
    s.peakBytes = gPeakBytes.load(std::memory_order_relaxed);
    s.failedAllocations = gFailures.load(std::memory_order_relaxed);
    return s;
}

void reset() {
    gTotalAlloc.store(0, std::memory_order_relaxed);
    gTotalDealloc.store(0, std::memory_order_relaxed);
    gLiveAlloc.store(0, std::memory_order_relaxed);
    gLiveBytes.store(0, std::memory_order_relaxed);
    gPeakBytes.store(0, std::memory_order_relaxed);
    gFailures.store(0, std::memory_order_relaxed);
}

void onAllocationFailure() {
    recordFailure();
    // Default behavior: rethrow bad_alloc. A real leak-detection tool would
    // log here; we let std::terminate handle it by throwing.
    throw std::bad_alloc();
}

}  // namespace allocation_tracker
