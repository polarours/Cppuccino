// examples/task-scheduler-error-handling.cpp
// Demonstrates TaskScheduler error handling: cancellation, timeout, and cleanup.
// Compile: g++ -std=c++20 -pthread -O2 -o task-scheduler-error-handling task-scheduler-error-handling.cpp

#include "../cpp-mastery-roadmap/05-stage5/task-scheduler/include/task_scheduler.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

int main() {
    using namespace task_scheduler;
    using Clock = std::chrono::steady_clock;
    using MS = std::chrono::milliseconds;

    std::cout << "=== Task Scheduler Error Handling Demo ===\n\n";

    TaskScheduler scheduler(2);

    // 1. Schedule tasks with different priorities
    auto id1 = scheduler.schedule([]() {
        std::cout << "[High] Task executed\n";
    }, TaskPriority::High);

    auto id2 = scheduler.schedule([]() {
        std::this_thread::sleep_for(MS(100));
        std::cout << "[Normal] Task executed\n";
    }, TaskPriority::Normal);

    // 2. Schedule recurring task
    auto id3 = scheduler.scheduleRepeating([]() {
        static int count = 0;
        if (++count < 3) {
            std::cout << "[Recurring] Tick " << count << "\n";
        }
    }, MS(50), TaskPriority::Normal);

    // 3. Schedule at specific time
    auto future = Clock::now() + MS(200);
    auto id4 = scheduler.scheduleAt([]() {
        std::cout << "[Delayed] Task executed after delay\n";
    }, future, TaskPriority::Low);

    scheduler.start();

    // Allow tasks to execute
    std::this_thread::sleep_for(MS(300));

    // 4. Cancel a task
    std::cout << "\nCancelling task " << id2 << "...\n";
    scheduler.cancel(id2);
    std::cout << "Pending tasks: " << scheduler.pendingTasks() << "\n";

    // Wait for remaining tasks
    std::this_thread::sleep_for(MS(300));

    std::cout << "\nCompleted tasks: " << scheduler.completedTasks() << "\n";

    scheduler.stop();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
