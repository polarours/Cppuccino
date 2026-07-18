#include "arena_allocator.hpp"

#include <iostream>

int main() {
    std::cout << "=== Arena Allocator Demo ===\n\n";

    arena_allocator::Arena arena(1024);
    std::cout << "Capacity: " << arena.capacity() << "\n";

    arena.allocate(100);
    arena.allocate(200);
    std::cout << "Used: " << arena.used() << ", Remaining: " << arena.remaining() << "\n";

    arena.reset();
    std::cout << "After reset - Used: " << arena.used() << ", Remaining: " << arena.remaining() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
