#include "thread_pool_advanced.hpp"

#include <iostream>

int main() {
    std::cout << "=== Thread Pool Advanced Demo ===\n\n";

    thread_pool_advanced::ThreadPool pool(4);

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 8; ++i) {
        futures.push_back(pool.submit([i]() { return i * i; }));
    }

    std::cout << "Results: ";
    for (auto& f : futures) {
        std::cout << f.get() << " ";
    }
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
