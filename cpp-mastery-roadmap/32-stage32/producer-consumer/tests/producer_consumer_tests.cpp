#include "producer_consumer.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <atomic>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_push_pop() {
    producer_consumer::BoundedQueue<int> queue(10);
    queue.push(1);
    queue.push(2);

    int item;
    expect(queue.pop(item) && item == 1, "expected 1");
    expect(queue.pop(item) && item == 2, "expected 2");
}

void test_size() {
    producer_consumer::BoundedQueue<int> queue(5);
    queue.push(1);
    queue.push(2);
    queue.push(3);

    expect(queue.size() == 3, "expected 3");
    expect(!queue.empty(), "expected not empty");
}

void test_bounded() {
    producer_consumer::BoundedQueue<int> queue(2);
    queue.push(1);
    queue.push(2);

    bool result = queue.push(3, std::chrono::milliseconds(50));
    expect(!result, "expected full queue rejection");
}

void test_concurrent() {
    producer_consumer::BoundedQueue<int> queue(10);
    std::atomic<int> sum{0};

    std::thread producer([&queue]() {
        for (int i = 1; i <= 10; ++i) {
            queue.push(i);
        }
    });

    std::thread consumer([&queue, &sum]() {
        for (int i = 0; i < 10; ++i) {
            int item;
            if (queue.pop(item)) {
                sum += item;
            }
        }
    });

    producer.join();
    consumer.join();

    expect(sum == 55, "expected sum 55");
}

} // namespace

int main() {
    try {
        std::cout << "Producer Consumer Tests:\n";
        test_basic_push_pop();
        std::cout << "  test_basic_push_pop: PASS\n";

        test_size();
        std::cout << "  test_size: PASS\n";

        test_bounded();
        std::cout << "  test_bounded: PASS\n";

        test_concurrent();
        std::cout << "  test_concurrent: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "producer_consumer_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nproducer_consumer_tests passed" << std::endl;
    return 0;
}
