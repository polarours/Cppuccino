#include "ring_buffer.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_push_pop() {
    ring_buffer::RingBuffer<int> buffer(5);

    expect(buffer.push(1), "expected push to succeed");
    expect(buffer.push(2), "expected push to succeed");
    expect(buffer.push(3), "expected push to succeed");

    expect(buffer.pop().value() == 1, "expected 1");
    expect(buffer.pop().value() == 2, "expected 2");
    expect(buffer.pop().value() == 3, "expected 3");
}

void test_buffer_full() {
    ring_buffer::RingBuffer<int> buffer(3);

    expect(buffer.push(1), "expected push to succeed");
    expect(buffer.push(2), "expected push to succeed");
    expect(buffer.push(3), "expected push to succeed");
    expect(!buffer.push(4), "expected push to fail when full");
}

void test_buffer_empty() {
    ring_buffer::RingBuffer<int> buffer(3);

    expect(buffer.empty(), "expected buffer to be empty");
    expect(!buffer.pop().has_value(), "expected pop to return nullopt");
}

void test_buffer_size() {
    ring_buffer::RingBuffer<int> buffer(5);

    expect(buffer.size() == 0, "expected size 0");
    buffer.push(1);
    expect(buffer.size() == 1, "expected size 1");
    buffer.push(2);
    expect(buffer.size() == 2, "expected size 2");
    buffer.pop();
    expect(buffer.size() == 1, "expected size 1");
}

void test_buffer_capacity() {
    ring_buffer::RingBuffer<int> buffer(10);
    expect(buffer.capacity() == 10, "expected capacity 10");
}

void test_peek() {
    ring_buffer::RingBuffer<int> buffer(5);

    expect(!buffer.peek().has_value(), "expected peek to return nullopt");

    buffer.push(42);
    expect(buffer.peek().value() == 42, "expected peek to return 42");
    expect(buffer.size() == 1, "expected size to remain 1");
}

void test_clear() {
    ring_buffer::RingBuffer<int> buffer(5);

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);

    buffer.clear();
    expect(buffer.empty(), "expected buffer to be empty");
    expect(buffer.size() == 0, "expected size 0");
}

void test_concurrent_producer_consumer() {
    ring_buffer::RingBuffer<int> buffer(10);
    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    std::thread producer([&]() {
        for (int i = 0; i < 100; ++i) {
            while (!buffer.push(i)) {
                std::this_thread::yield();
            }
            produced++;
        }
    });

    std::thread consumer([&]() {
        while (consumed < 100) {
            auto item = buffer.pop();
            if (item) {
                consumed++;
            }
        }
    });

    producer.join();
    consumer.join();

    expect(produced == 100, "expected 100 produced");
    expect(consumed == 100, "expected 100 consumed");
}

void test_fifo_order() {
    ring_buffer::RingBuffer<int> buffer(5);

    for (int i = 0; i < 5; ++i) {
        buffer.push(i);
    }

    for (int i = 0; i < 5; ++i) {
        expect(buffer.pop().value() == i, "expected FIFO order");
    }
}

void test_wrap_around() {
    ring_buffer::RingBuffer<int> buffer(3);

    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.pop();
    buffer.pop();
    buffer.push(4);
    buffer.push(5);

    expect(buffer.pop().value() == 3, "expected 3");
    expect(buffer.pop().value() == 4, "expected 4");
    expect(buffer.pop().value() == 5, "expected 5");
}

} // namespace

int main() {
    try {
        std::cout << "Ring Buffer Tests:\n";
        test_basic_push_pop();
        std::cout << "  test_basic_push_pop: PASS\n";

        test_buffer_full();
        std::cout << "  test_buffer_full: PASS\n";

        test_buffer_empty();
        std::cout << "  test_buffer_empty: PASS\n";

        test_buffer_size();
        std::cout << "  test_buffer_size: PASS\n";

        test_buffer_capacity();
        std::cout << "  test_buffer_capacity: PASS\n";

        test_peek();
        std::cout << "  test_peek: PASS\n";

        test_clear();
        std::cout << "  test_clear: PASS\n";

        test_concurrent_producer_consumer();
        std::cout << "  test_concurrent_producer_consumer: PASS\n";

        test_fifo_order();
        std::cout << "  test_fifo_order: PASS\n";

        test_wrap_around();
        std::cout << "  test_wrap_around: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "ring_buffer_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nring_buffer_tests passed" << std::endl;
    return 0;
}
