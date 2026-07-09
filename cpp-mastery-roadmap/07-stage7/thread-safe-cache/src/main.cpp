#include "thread_safe_cache.hpp"

#include <iostream>
#include <string>
#include <thread>

int main() {
    std::cout << "=== Thread-Safe Cache Demo ===\n\n";

    thread_safe_cache::Cache<std::string, int> cache(10);

    // Basic operations
    cache.set("users", 100);
    cache.set("posts", 500);
    cache.set("comments", 1200);

    std::cout << "users: " << cache.getOrElse("users", 0) << "\n";
    std::cout << "posts: " << cache.getOrElse("posts", 0) << "\n";
    std::cout << "comments: " << cache.getOrElse("comments", 0) << "\n";
    std::cout << "missing: " << cache.getOrElse("missing", -1) << "\n";

    // TTL
    cache.set("temp", 42, std::chrono::seconds(1));
    std::cout << "temp (immediate): " << cache.getOrElse("temp", -1) << "\n";

    // Concurrent access
    std::cout << "\nConcurrent access test:\n";
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&cache, i]() {
            for (int j = 0; j < 10; ++j) {
                cache.set("thread_" + std::to_string(i) + "_" + std::to_string(j), j);
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "Cache size: " << cache.size() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
