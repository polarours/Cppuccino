#include "slab_allocator.hpp"

#include <iostream>

int main() {
    std::cout << "=== Slab Allocator Demo ===\n\n";

    slab_allocator::SlabAllocator alloc(1024);
    std::cout << "Slab size: " << alloc.slabSize() << "\n";

    void* p1 = alloc.allocate(32);
    void* p2 = alloc.allocate(64);
    void* p3 = alloc.allocate(128);

    std::cout << "Allocated 3 blocks: " << p1 << ", " << p2 << ", " << p3 << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
