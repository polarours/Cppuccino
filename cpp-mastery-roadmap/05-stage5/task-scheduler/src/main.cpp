#include "task_scheduler.hpp"

#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== Task Scheduler Demo ===\n\n";

    task_scheduler::TaskScheduler scheduler(4);
    scheduler.start();

    std::cout << "Scheduling immediate tasks...\n";
    for (int i = 0; i < 5; ++i) {
        scheduler.schedule([i]() {
            std::cout << "  Task " << i << " executed\n";
        });
    }

    std::cout << "\nScheduling priority tasks...\n";
    scheduler.schedule([]() { std::cout << "  Low priority\n"; }, task_scheduler::TaskPriority::Low);
    scheduler.schedule([]() { std::cout << "  High priority\n"; }, task_scheduler::TaskPriority::High);
    scheduler.schedule([]() { std::cout << "  Normal priority\n"; }, task_scheduler::TaskPriority::Normal);

    std::cout << "\nScheduling repeating task...\n";
    int count = 0;
    auto repeatId = scheduler.scheduleRepeating([&count]() {
        count++;
        std::cout << "  Repeat " << count << "\n";
    }, std::chrono::milliseconds(50));

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    scheduler.cancel(repeatId);

    std::cout << "\nWaiting for tasks to complete...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    scheduler.stop();

    std::cout << "\nCompleted tasks: " << scheduler.completedTasks() << "\n";
    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
