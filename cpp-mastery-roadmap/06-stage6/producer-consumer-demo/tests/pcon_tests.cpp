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

} // namespace

int main() {
    try {
        test_thread_safe_ring_buffer();
        std::cout << "pcon_tests passed\n";
    } catch (const std::exception& e) {
        std::cerr << "pcon_tests failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
