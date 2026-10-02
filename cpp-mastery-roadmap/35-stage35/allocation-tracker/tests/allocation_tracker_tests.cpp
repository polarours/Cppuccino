#include "allocation_tracker.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_counter_monotonic() {
    allocation_tracker::reset();
    allocation_tracker::recordAllocation(100);
    auto s = allocation_tracker::snapshot();
    expect(s.totalAllocations == 1, "1 allocation");
    expect(s.liveAllocations == 1, "1 live");
    expect(s.liveBytes == 100, "100 bytes");
    expect(s.peakBytes == 100, "peak 100");

    allocation_tracker::recordAllocation(50);
    allocation_tracker::recordDeallocation(100);
    s = allocation_tracker::snapshot();
    expect(s.totalAllocations == 2, "2 allocations");
    expect(s.totalDeallocations == 1, "1 dealloc");
    expect(s.liveAllocations == 1, "1 live left");
    expect(s.liveBytes == 50, "50 bytes left");
    expect(s.peakBytes == 150, "peak remembered 150");
}

void test_reset() {
    allocation_tracker::reset();
    allocation_tracker::recordAllocation(10);
    allocation_tracker::reset();
    auto s = allocation_tracker::snapshot();
    expect(s.totalAllocations == 0 && s.liveBytes == 0, "reset clears");
}

void test_failure_counter() {
    allocation_tracker::reset();
    allocation_tracker::recordFailure();
    auto s = allocation_tracker::snapshot();
    expect(s.failedAllocations == 1, "failure recorded");
}

void test_counting_allocator_vector_roundtrip() {
    allocation_tracker::reset();
    {
        std::vector<int, allocation_tracker::CountingAllocator<int>> v;
        for (int i = 0; i < 100; ++i) v.push_back(i * 2);
        auto s = allocation_tracker::snapshot();
        expect(s.totalAllocations > 0, "vector allocated");
        expect(s.liveAllocations > 0, "still live inside scope");
        expect(s.liveBytes >= 100 * sizeof(int), "at least the ints");
    }
    auto s = allocation_tracker::snapshot();
    expect(s.liveAllocations == 0, "all freed after scope");
    expect(s.liveBytes == 0, "bytes returned");
    expect(s.totalAllocations == s.totalDeallocations, "alloc == dealloc");
}

void test_allocators_compare_equal() {
    allocation_tracker::CountingAllocator<int> a;
    allocation_tracker::CountingAllocator<double> b;
    expect(a == b, "stateless allocators always equal (rebind-safe)");
    expect(!(a != b), "operator!= consistent");
}

void test_peak_survives_free() {
    allocation_tracker::reset();
    allocation_tracker::recordAllocation(1000);
    allocation_tracker::recordDeallocation(1000);
    auto s = allocation_tracker::snapshot();
    expect(s.liveBytes == 0, "live back to 0");
    expect(s.peakBytes == 1000, "peak still recorded");
}

}  // namespace

int main() {
    try {
        test_counter_monotonic();
        test_reset();
        test_failure_counter();
        test_counting_allocator_vector_roundtrip();
        test_allocators_compare_equal();
        test_peak_survives_free();
        std::cout << "All allocation_tracker tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
