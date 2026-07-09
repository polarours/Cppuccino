#include "ring_buffer.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

int main() {
    std::cout << "=== Ring Buffer Demo ===\n\n";

    ring_buffer::RingBuffer<std::string> buffer(5);

    // Producer
    std::thread producer([&buffer]() {
        for (int i = 0; i < 10; ++i) {
            std::string item = "item_" + std::to_string(i);
            while (!buffer.push(item)) {
                std::this_thread::yield();
            }
            std::cout << "Produced: " << item << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    // Consumer
    std::thread consumer([&buffer]() {
        int consumed = 0;
        while (consumed < 10) {
            auto item = buffer.popWait(std::chrono::milliseconds(100));
            if (item) {
                std::cout << "Consumed: " << *item << "\n";
                consumed++;
            }
        }
    });

    producer.join();
    consumer.join();

    std::cout << "\nBuffer size: " << buffer.size() << "\n";
    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
