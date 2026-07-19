#include "memory_alignment.hpp"

#include <cstdint>
#include <iostream>

int main() {
    std::cout << "=== Memory Alignment Demo ===\n\n";

    memory_alignment::CacheLineAligned obj;
    auto addr = reinterpret_cast<std::uintptr_t>(&obj);
    std::cout << "CacheLineAligned addr: " << addr << " (aligned to 64: " << (addr % 64 == 0) << ")\n";

    memory_alignment::AlignedStorage<int, 64> storage;
    storage.get() = 42;
    auto saddr = reinterpret_cast<std::uintptr_t>(&storage);
    std::cout << "AlignedStorage addr: " << saddr << " (aligned to 64: " << (saddr % 64 == 0) << ")\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
