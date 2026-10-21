#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include "ring_buffer.hpp"
#include <stdexcept>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// Test that ring buffer is thread-safe for concurrent push/pop
void test_thread_safe_ring_buffer() {
    ring_buffer::RingBuffer<int> buffer(10);
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    // Multiple producers
    for (int i = 0; i < 3; ++i) {
        producers.emplace_back([&buffer, &produced]() {
            for (int j = 0; j < 10; ++j) {
                while (!buffer.push(j)) {
                    std::this_thread::yield();
                }
                produced++;
            }
        });
    }

    // Multiple consumers
    for (int i = 0; i < 3; ++i) {
        consumers.emplace_back([&buffer, &consumed]() {
            int local_count = 0;
            while (local_count < 10) {
                auto val = buffer.pop();
                if (val.has_value()) {
                    local_count++;
                    consumed++;
                }
            }
        });
    }

    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    expect(produced.load() == 30, "expected 30 produced items");
    expect(consumed.load() == 30, "expected 30 consumed items");
    expect(buffer.empty(), "buffer should be empty");
}

// Test that the buffer reports its state correctly
void test_basic_push_pop() {
    ring_buffer::RingBuffer<int> buffer(3);

    expect(buffer.empty(), "a fresh buffer is empty");
    expect(buffer.size() == 0, "a fresh buffer has size 0");
    expect(buffer.capacity() == 3, "capacity is preserved");
    expect(!buffer.pop().has_value(), "popping an empty buffer yields nothing");
    expect(!buffer.peek().has_value(), "peeking an empty buffer yields nothing");

    expect(buffer.push(1), "pushing into a free buffer succeeds");
    expect(buffer.push(2), "pushing into a free buffer succeeds");
    expect(buffer.size() == 2, "size reflects the two pushes");
    expect(!buffer.empty(), "the buffer is not empty");
    expect(buffer.peek().has_value(), "peeking a non-empty buffer yields a value");
    expect(buffer.peek().value() == 1, "peek returns the oldest item");

    auto first = buffer.pop();
    expect(first.has_value(), "popping a non-empty buffer yields a value");
    expect(first.value() == 1, "pop returns the oldest item");
    expect(buffer.size() == 1, "size dropped after the pop");
}

// Test that a full buffer refuses further pushes and can be cleared
void test_full_buffer_and_clear() {
    ring_buffer::RingBuffer<int> buffer(2);

    expect(buffer.push(10), "first push succeeds");
    expect(buffer.push(20), "second push succeeds");
    expect(buffer.full(), "the buffer reports itself full");
    expect(!buffer.push(30), "pushing into a full buffer fails");
    expect(buffer.size() == 2, "the failed push did not change the size");

    buffer.clear();
    expect(buffer.empty(), "clear empties the buffer");
    expect(buffer.size() == 0, "clear resets the size");
    expect(!buffer.full(), "the cleared buffer is not full");
    expect(buffer.push(30), "the cleared buffer accepts a push again");
    expect(buffer.pop().value() == 30, "the item pushed after clear comes back");
}

} // namespace

int main() {
    try {
        test_basic_push_pop();
        test_full_buffer_and_clear();
        test_thread_safe_ring_buffer();
        std::cout << "pcon_tests passed\n";
    } catch (const std::exception& e) {
        std::cerr << "pcon_tests failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
