#include "producer_consumer.hpp"

#include <iostream>
#include <thread>

int main() {
    std::cout << "=== Producer Consumer Demo ===\n\n";

    producer_consumer::BoundedQueue<int> queue(5);

    std::thread producer([&queue]() {
        for (int i = 1; i <= 5; ++i) {
            queue.push(i);
            std::cout << "Produced: " << i << "\n";
        }
    });

    std::thread consumer([&queue]() {
        for (int i = 0; i < 5; ++i) {
            int item;
            if (queue.pop(item)) {
                std::cout << "Consumed: " << item << "\n";
            }
        }
    });

    producer.join();
    consumer.join();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
