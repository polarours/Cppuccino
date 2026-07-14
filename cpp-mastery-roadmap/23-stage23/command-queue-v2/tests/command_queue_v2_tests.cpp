#include "command_queue_v2.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_enqueue() {
    command_queue_v2::CommandQueue queue(2);
    std::atomic<int> counter{0};

    queue.enqueue([&counter]() { counter++; });
    queue.enqueue([&counter]() { counter++; });

    queue.waitForAll();
    expect(counter == 2, "expected 2 tasks");
}

void test_priority() {
    command_queue_v2::CommandQueue queue(1);
    std::vector<int> order;

    queue.enqueue([&order]() { order.push_back(1); }, command_queue_v2::Priority::Low);
    queue.enqueue([&order]() { order.push_back(2); }, command_queue_v2::Priority::High);
    queue.enqueue([&order]() { order.push_back(3); }, command_queue_v2::Priority::Normal);

    queue.waitForAll();
    expect(order[0] == 2, "expected high priority first");
    expect(order[1] == 3, "expected normal priority second");
    expect(order[2] == 1, "expected low priority last");
}

void test_concurrent() {
    command_queue_v2::CommandQueue queue(4);
    std::atomic<int> counter{0};

    for (int i = 0; i < 20; ++i) {
        queue.enqueue([&counter]() { counter++; });
    }

    queue.waitForAll();
    expect(counter == 20, "expected 20 tasks");
}

} // namespace

int main() {
    try {
        std::cout << "Command Queue V2 Tests:\n";
        test_basic_enqueue();
        std::cout << "  test_basic_enqueue: PASS\n";

        test_priority();
        std::cout << "  test_priority: PASS\n";

        test_concurrent();
        std::cout << "  test_concurrent: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_queue_v2_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_queue_v2_tests passed" << std::endl;
    return 0;
}
