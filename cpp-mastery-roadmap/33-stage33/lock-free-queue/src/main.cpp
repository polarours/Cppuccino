#include "lock_free_queue.hpp"

#include <iostream>
#include <thread>

int main() {
    std::cout << "=== Lock Free Queue Demo ===\n\n";

    lock_free_queue::LockFreeQueue<int> queue;

    std::thread producer([&queue]() {
        for (int i = 1; i <= 5; ++i) {
            queue.push(i);
            std::cout << "Pushed: " << i << "\n";
        }
    });

    std::thread consumer([&queue]() {
        for (int i = 0; i < 5; ++i) {
            auto val = queue.pop();
            if (val) std::cout << "Popped: " << *val << "\n";
        }
    });

    producer.join();
    consumer.join();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
