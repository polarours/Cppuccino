#include "simple_allocator.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <string>

int main() {
    std::cout << "=== Simple Allocator Demo ===\n\n";

    // Create allocator with 64-byte blocks, 8 blocks total
    simple_allocator::SimpleAllocator allocator(64, 8);

    std::cout << "Block size: " << allocator.blockSize() << " bytes\n";
    std::cout << "Block count: " << allocator.blockCount() << "\n";
    std::cout << "Free blocks: " << allocator.freeBlocks() << "\n\n";

    // Allocate some memory
    std::cout << "Allocating memory...\n";
    void* ptr1 = allocator.allocate(32);
    std::cout << "  ptr1: " << ptr1 << " (used: " << allocator.usedBlocks() << ")\n";

    void* ptr2 = allocator.allocate(32);
    std::cout << "  ptr2: " << ptr2 << " (used: " << allocator.usedBlocks() << ")\n";

    void* ptr3 = allocator.allocate(32);
    std::cout << "  ptr3: " << ptr3 << " (used: " << allocator.usedBlocks() << ")\n";

    // Deallocate
    std::cout << "\nDeallocating ptr2...\n";
    allocator.deallocate(ptr2);
    std::cout << "  Free blocks: " << allocator.freeBlocks() << "\n";

    // Allocate again (reuses freed block)
    std::cout << "\nAllocating again...\n";
    void* ptr4 = allocator.allocate(32);
    std::cout << "  ptr4: " << ptr4 << " (same as ptr2? " << (ptr4 == ptr2 ? "yes" : "no") << ")\n";

    // Cleanup
    allocator.deallocate(ptr1);
    allocator.deallocate(ptr3);
    allocator.deallocate(ptr4);

    std::cout << "\n--- STL Allocator Demo ---\n\n";

    simple_allocator::SimpleAllocator stlAllocator(sizeof(int) * 100, 100);
    simple_allocator::StlAllocator<int> alloc(stlAllocator);

    std::vector<int, simple_allocator::StlAllocator<int>> vec(alloc);
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i * 10);
    }

    std::cout << "Vector: ";
    for (int val : vec) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
