#include "lru_cache.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== LRU Cache Demo ===\n\n";

    lru_cache::LRUCache<std::string, int> cache(3);

    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("c", 3);

    std::cout << "a: " << cache.getOrElse("a", 0) << "\n";
    std::cout << "b: " << cache.getOrElse("b", 0) << "\n";
    std::cout << "c: " << cache.getOrElse("c", 0) << "\n";

    // Access "a" to make it most recently used
    cache.get("a");

    // Add new entry, should evict "b"
    cache.put("d", 4);
    std::cout << "\nAfter adding d:\n";
    std::cout << "a: " << cache.getOrElse("a", 0) << "\n";
    std::cout << "b: " << cache.getOrElse("b", -1) << " (evicted)\n";
    std::cout << "c: " << cache.getOrElse("c", 0) << "\n";
    std::cout << "d: " << cache.getOrElse("d", 0) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
