#include "strategy_cache.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Strategy Cache Demo ===\n\n";

    strategy_cache::Cache<std::string, int> cache;

    // Use LRU strategy
    cache.setStrategy(std::make_shared<strategy_cache::LRUStrategy<std::string, int>>(3));

    cache.put("key1", 100);
    cache.put("key2", 200);
    cache.put("key3", 300);

    std::cout << "LRU Cache:\n";
    std::cout << "  key1: " << cache.get("key1").value_or(-1) << "\n";
    std::cout << "  size: " << cache.size() << "\n";

    // Add one more, should evict oldest
    cache.put("key4", 400);
    std::cout << "  After adding key4: size = " << cache.size() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
