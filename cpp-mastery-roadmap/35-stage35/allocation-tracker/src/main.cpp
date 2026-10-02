#include "allocation_tracker.hpp"

#include <iostream>
#include <vector>

int main() {
    allocation_tracker::reset();

    {
        std::vector<int, allocation_tracker::CountingAllocator<int>> v;
        for (int i = 0; i < 1000; ++i) v.push_back(i);

        auto s = allocation_tracker::snapshot();
        std::cout << "inside scope: allocations=" << s.totalAllocations
                  << " live=" << s.liveAllocations
                  << " liveBytes=" << s.liveBytes
                  << " peak=" << s.peakBytes << "\n";
    }
    auto s = allocation_tracker::snapshot();
    std::cout << "after scope: live=" << s.liveAllocations
              << " liveBytes=" << s.liveBytes
              << " (expect 0 / 0)\n";
    return 0;
}
