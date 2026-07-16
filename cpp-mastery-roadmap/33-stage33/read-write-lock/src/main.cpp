#include "read_write_lock.hpp"

#include <iostream>
#include <thread>
#include <vector>

int main() {
    std::cout << "=== Read Write Lock Demo ===\n\n";

    read_write_lock::ThreadSafeMap<std::string, int> map;

    std::vector<std::thread> writers;
    for (int i = 0; i < 5; ++i) {
        writers.emplace_back([&map, i]() {
            map.insert("key" + std::to_string(i), i * 10);
        });
    }
    for (auto& t : writers) t.join();

    std::cout << "Map size: " << map.size() << "\n";
    for (const auto& k : map.keys()) {
        std::cout << "  " << k << " = " << map.get(k) << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
