#include "command_queue.hpp"

#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== Command Queue Demo ===\n\n";

    command_queue::CommandQueue queue(4);
    std::atomic<int> counter{0};

    std::cout << "Enqueueing 20 tasks...\n";

    for (int i = 0; i < 20; ++i) {
        queue.enqueue([&counter, i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            counter++;
        });
    }

    queue.waitForAll();
    std::cout << "Completed: " << counter << " tasks\n";
    std::cout << "Total completed: " << queue.completedTasks() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
