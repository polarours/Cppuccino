#include "thread_pool.hpp"

#include <iostream>
#include <string>
#include <chrono>

int main() {
    std::cout << "=== Thread Pool Demo ===\n\n";

    // Create thread pool with 4 threads
    thread_pool::ThreadPool pool(4);
    std::cout << "Created thread pool with " << pool.threadCount() << " threads\n\n";

    // Enqueue simple tasks
    std::cout << "Enqueueing tasks...\n";
    auto start = std::chrono::steady_clock::now();

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 8; ++i) {
        futures.push_back(pool.enqueue([i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return i * i;
        }));
    }

    // Get results
    std::cout << "\nResults:\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << "  " << i << "^2 = " << futures[i].get() << "\n";
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nTotal time: " << duration.count() << "ms\n";
    std::cout << "(Without parallelism would be ~800ms)\n";

    // Task with arguments
    std::cout << "\nTask with arguments:\n";
    auto sumFuture = pool.enqueue([](int a, int b, int c) {
        return a + b + c;
    }, 10, 20, 30);
    std::cout << "  10 + 20 + 30 = " << sumFuture.get() << "\n";

    // Wait for all tasks
    std::cout << "\nWaiting for all tasks...\n";
    pool.waitForAll();
    std::cout << "All tasks completed\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
