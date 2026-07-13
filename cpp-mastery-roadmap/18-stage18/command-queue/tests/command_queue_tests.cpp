#include "command_queue.hpp"

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
    command_queue::CommandQueue queue(2);
    std::atomic<int> counter{0};

    queue.enqueue([&counter]() { counter++; });
    queue.enqueue([&counter]() { counter++; });
    queue.enqueue([&counter]() { counter++; });

    queue.waitForAll();
    expect(counter == 3, "expected 3 tasks");
    expect(queue.completedTasks() == 3, "expected 3 completed");
}

void test_concurrent_enqueue() {
    command_queue::CommandQueue queue(4);
    std::atomic<int> counter{0};

    for (int i = 0; i < 20; ++i) {
        queue.enqueue([&counter]() {
            counter++;
        });
    }

    queue.waitForAll();
    expect(counter == 20, "expected 20 tasks");
}

void test_shutdown() {
    command_queue::CommandQueue queue(2);
    std::atomic<int> counter{0};

    queue.enqueue([&counter]() { counter++; });
    queue.enqueue([&counter]() { counter++; });

    queue.shutdown();
    expect(queue.isShutdown(), "expected shutdown");
}

void test_pending_tasks() {
    command_queue::CommandQueue queue(1);
    expect(queue.pendingTasks() == 0, "expected 0 pending");
}

} // namespace

int main() {
    try {
        std::cout << "Command Queue Tests:\n";
        test_basic_enqueue();
        std::cout << "  test_basic_enqueue: PASS\n";

        test_concurrent_enqueue();
        std::cout << "  test_concurrent_enqueue: PASS\n";

        test_shutdown();
        std::cout << "  test_shutdown: PASS\n";

        test_pending_tasks();
        std::cout << "  test_pending_tasks: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_queue_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_queue_tests passed" << std::endl;
    return 0;
}
