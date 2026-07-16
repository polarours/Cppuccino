#include "lock_free_queue.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_push_pop() {
    lock_free_queue::LockFreeQueue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);

    expect(queue.pop().value() == 1, "expected 1");
    expect(queue.pop().value() == 2, "expected 2");
    expect(queue.pop().value() == 3, "expected 3");
    expect(!queue.pop().has_value(), "expected empty");
}

void test_empty() {
    lock_free_queue::LockFreeQueue<int> queue;
    expect(queue.empty(), "expected empty");

    queue.push(1);
    expect(!queue.empty(), "expected not empty");

    queue.pop();
    expect(queue.empty(), "expected empty again");
}

void test_concurrent_push_pop() {
    lock_free_queue::LockFreeQueue<int> queue;
    std::atomic<int> sum{0};

    std::thread producer([&queue]() {
        for (int i = 1; i <= 100; ++i) {
            queue.push(i);
        }
    });

    std::thread consumer([&queue, &sum]() {
        for (int i = 0; i < 100; ++i) {
            auto val = queue.pop();
            if (val.has_value()) sum += *val;
        }
    });

    producer.join();
    consumer.join();

    expect(sum == 5050, "expected sum 5050");
}

void test_multi_producer_consumer() {
    lock_free_queue::LockFreeQueue<int> queue;
    std::atomic<int> consumed{0};

    std::vector<std::thread> producers;
    for (int i = 0; i < 4; ++i) {
        producers.emplace_back([&queue, i]() {
            for (int j = 0; j < 25; ++j) {
                queue.push(i * 25 + j + 1);
            }
        });
    }
    for (auto& t : producers) t.join();

    std::vector<std::thread> consumers;
    for (int i = 0; i < 4; ++i) {
        consumers.emplace_back([&queue, &consumed]() {
            for (int j = 0; j < 25; ++j) {
                auto val = queue.pop();
                if (val.has_value()) consumed++;
            }
        });
    }
    for (auto& t : consumers) t.join();

    expect(consumed == 100, "expected 100 consumed");
}

} // namespace

int main() {
    try {
        std::cout << "Lock Free Queue Tests:\n";
        test_push_pop();
        std::cout << "  test_push_pop: PASS\n";

        test_empty();
        std::cout << "  test_empty: PASS\n";

        test_concurrent_push_pop();
        std::cout << "  test_concurrent_push_pop: PASS\n";

        test_multi_producer_consumer();
        std::cout << "  test_multi_producer_consumer: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "lock_free_queue_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nlock_free_queue_tests passed" << std::endl;
    return 0;
}
