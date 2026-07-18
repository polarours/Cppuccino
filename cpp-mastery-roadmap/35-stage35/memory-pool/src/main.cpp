#include "memory_pool.hpp"

#include <iostream>

int main() {
    std::cout << "=== Memory Pool Demo ===\n\n";

    memory_pool::MemoryPool pool(sizeof(int), 8);
    std::cout << "Pool: " << pool.totalBlocks() << " blocks, " << pool.blockSize() << " bytes each\n";

    void* p1 = pool.allocate();
    void* p2 = pool.allocate();
    std::cout << "Allocated 2: " << pool.usedBlocks() << " used, " << pool.freeBlocks() << " free\n";

    pool.deallocate(p1);
    pool.deallocate(p2);
    std::cout << "Deallocated: " << pool.usedBlocks() << " used, " << pool.freeBlocks() << " free\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
