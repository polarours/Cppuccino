#include "command_queue_v2.hpp"

#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== Command Queue V2 Demo ===\n\n";

    command_queue_v2::CommandQueue queue(4);
    std::atomic<int> counter{0};

    std::cout << "Enqueueing tasks with priorities...\n";

    queue.enqueue([&counter]() { counter++; }, command_queue_v2::Priority::Low);
    queue.enqueue([&counter]() { counter++; }, command_queue_v2::Priority::High);
    queue.enqueue([&counter]() { counter++; }, command_queue_v2::Priority::Normal);

    queue.waitForAll();
    std::cout << "Completed: " << counter << " tasks\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
