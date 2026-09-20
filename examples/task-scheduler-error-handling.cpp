// examples/task-scheduler-error-handling.cpp
// Demonstrates TaskScheduler error handling: cancellation, exception safety,
// and graceful shutdown. Verifies behavior with asserts.
//
// Compile + run:
//   g++ -std=c++17 -pthread -Wall -Wextra -Werror -Icpp-mastery-roadmap/05-stage5/task-scheduler/include
//       -o /tmp/task-scheduler-error-handling examples/task-scheduler-error-handling.cpp
//       cpp-mastery-roadmap/05-stage5/task-scheduler/src/task_scheduler.cpp
//   /tmp/task-scheduler-error-handling

#include "task_scheduler.hpp"

#include <atomic>
#include <cassert>
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

    // 1. A task that throws must NOT kill the worker thread.
    auto throwingId = scheduler.schedule([]() {
        throw std::runtime_error("expected failure");
    }, TaskPriority::High);

    // 2. A task scheduled while a throwing task is in flight.
    auto id2 = scheduler.schedule([]() {
        std::cout << "[Normal] Task executed\n";
    }, TaskPriority::Normal);

    // 3. Recurring task: cancel it to stop the ticks.
    auto id3 = scheduler.scheduleRepeating([]() {
        std::cout << "[Recurring] Tick\n";
    }, MS(50), TaskPriority::Normal);

    // 4. Delayed task: cancel it so it never runs.
    auto id4 = scheduler.scheduleAt([]() {
        std::cout << "[Delayed] This should never print\n";
    }, Clock::now() + MS(500), TaskPriority::Low);

    scheduler.start();
    scheduler.cancel(id4); // cancel before it can fire
    std::this_thread::sleep_for(MS(300));
    scheduler.cancel(id3); // stop the recurring task
    scheduler.stop();

    // The throwing task must have been "completed" (its exception swallowed
    // and logged), so it counts in completedTasks.
    auto completed = scheduler.completedTasks();
    assert(completed >= 2);
    std::cout << "\nCompleted tasks: " << completed << " (>= 2 expected)\n";

    // 5. Fresh scheduler: cancel delayed task, verify it never ran.
    TaskScheduler scheduler2(1);
    std::atomic<int> delayedRuns{0};
    auto delayedId = scheduler2.scheduleAt([&delayedRuns]() {
        delayedRuns++;
    }, Clock::now() + MS(300), TaskPriority::Low);
    scheduler2.start();
    scheduler2.cancel(delayedId);
    std::this_thread::sleep_for(MS(600));
    scheduler2.stop();
    assert(delayedRuns == 0);
    std::cout << "Cancelled delayed task ran: " << delayedRuns.load()
              << " times (0 expected)\n";

    (void)throwingId;
    (void)id2;
    (void)id3;

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
