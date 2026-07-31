#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include "../ring-buffer/include/ring_buffer.hpp"

// Producer-consumer demo using the thread-safe ring buffer
// This demonstrates the classic producer-consumer pattern
// using the ring_buffer::RingBuffer as the shared buffer.

void producer(int id, ring_buffer::RingBuffer<int>& buffer, int num_items, std::atomic<int>& produced) {
    for (int i = 0; i < num_items; ++i) {
        int value = id * 1000 + i;  // Unique value with producer ID

        // If buffer is full, wait briefly and try again (backpressure)
        while (!buffer.push(value)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        produced++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));  // Simulate work
    }
    std::cout << "Producer " << id << " finished (produced " << num_items << " items)\n";
}

void consumer(int id, ring_buffer::RingBuffer<int>& buffer, int expected_items, std::atomic<int>& consumed) {
    int local_count = 0;
    while (local_count < expected_items) {
        auto value = buffer.popWait(std::chrono::milliseconds(100));
        if (value.has_value()) {
            std::cout << "Consumer " << id << " received: " << value.value() << "\n";
            local_count++;
            consumed++;
            std::this_thread::sleep_for(std::chrono::milliseconds(15));  // Simulate work
        } else {
            // Timeout but still need more items
            if (local_count < expected_items) {
                // Don't spam "waiting" messages too frequently
                if (local_count % 10 == 0) {
                    std::cout << "Consumer " << id << " waiting for more data...\n";
                }
            }
        }
    }
    std::cout << "Consumer " << id << " finished (consumed " << local_count << " items)\n";
}

int main() {
    std::cout << "========== Producer-Consumer Demo ==========\n\n";

    const int BUFFER_SIZE = 10;  // Increased buffer to reduce contention
    const int NUM_PRODUCERS = 2;
    const int NUM_CONSUMERS = 2;
    const int ITEMS_PER_PRODUCER = 10;
    const int ITEMS_PER_CONSUMER = ITEMS_PER_PRODUCER * NUM_PRODUCERS / NUM_CONSUMERS;

    ring_buffer::RingBuffer<int> buffer(BUFFER_SIZE);

    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    // Create producers
    std::vector<std::thread> producers;
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(producer, i, std::ref(buffer), ITEMS_PER_PRODUCER, std::ref(produced));
    }

    // Create consumers
    std::vector<std::thread> consumers;
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back(consumer, i, std::ref(buffer), ITEMS_PER_CONSUMER, std::ref(consumed));
    }

    // Wait for all producers to finish
    for (auto& t : producers) {
        t.join();
    }

    // Wait for all consumers to finish
    for (auto& t : consumers) {
        t.join();
    }

    std::cout << "\n========== Results ==========\n";
    std::cout << "Total produced: " << produced.load() << "\n";
    std::cout << "Total consumed: " << consumed.load() << "\n";
    std::cout << "Buffer final size: " << buffer.size() << "\n";
    std::cout << "Buffer capacity: " << buffer.capacity() << "\n";

    if (produced.load() == consumed.load()) {
        std::cout << "\n*** All items successfully produced and consumed ***\n";
    } else {
        std::cout << "\n*** Warning: Items produced != consumed ***\n";
    }

    return 0;
}
