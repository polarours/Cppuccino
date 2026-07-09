#include "task_scheduler.hpp"

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

void test_basic_schedule() {
    task_scheduler::TaskScheduler scheduler(2);
    std::atomic<int> counter{0};

    scheduler.start();

    scheduler.schedule([&counter]() {
        counter++;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.stop();

    expect(counter == 1, "expected task to execute");
}

void test_multiple_tasks() {
    task_scheduler::TaskScheduler scheduler(2);
    std::atomic<int> counter{0};

    scheduler.start();

    for (int i = 0; i < 10; ++i) {
        scheduler.schedule([&counter]() {
            counter++;
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    scheduler.stop();

    expect(counter == 10, "expected 10 tasks executed");
}

void test_priority() {
    task_scheduler::TaskScheduler scheduler(1);
    std::vector<int> executionOrder;

    scheduler.start();

    scheduler.schedule([&]() { executionOrder.push_back(1); }, task_scheduler::TaskPriority::Low);
    scheduler.schedule([&]() { executionOrder.push_back(2); }, task_scheduler::TaskPriority::High);
    scheduler.schedule([&]() { executionOrder.push_back(3); }, task_scheduler::TaskPriority::Normal);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.stop();

    expect(executionOrder.size() == 3, "expected 3 tasks");
    expect(executionOrder[0] == 2, "expected high priority first");
    expect(executionOrder[1] == 3, "expected normal priority second");
    expect(executionOrder[2] == 1, "expected low priority last");
}

void test_cancel() {
    task_scheduler::TaskScheduler scheduler(2);
    std::atomic<int> counter{0};

    scheduler.start();

    auto id = scheduler.schedule([&counter]() {
        counter++;
    });

    scheduler.cancel(id);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.stop();

    expect(counter == 0, "expected cancelled task not to execute");
}

void test_repeating() {
    task_scheduler::TaskScheduler scheduler(2);
    std::atomic<int> counter{0};

    scheduler.start();

    scheduler.scheduleRepeating([&counter]() {
        counter++;
    }, std::chrono::milliseconds(10));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.stop();

    expect(counter >= 5, "expected at least 5 executions");
}

void test_completed_count() {
    task_scheduler::TaskScheduler scheduler(2);

    scheduler.start();

    for (int i = 0; i < 5; ++i) {
        scheduler.schedule([]() {});
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.stop();

    expect(scheduler.completedTasks() == 5, "expected 5 completed tasks");
}

void test_pending_tasks() {
    task_scheduler::TaskScheduler scheduler(2);

    expect(scheduler.pendingTasks() == 0, "expected 0 pending tasks");

    scheduler.start();
    scheduler.schedule([]() {});

    scheduler.stop();
}

} // namespace

int main() {
    try {
        std::cout << "Task Scheduler Tests:\n";
        test_basic_schedule();
        std::cout << "  test_basic_schedule: PASS\n";

        test_multiple_tasks();
        std::cout << "  test_multiple_tasks: PASS\n";

        test_priority();
        std::cout << "  test_priority: PASS\n";

        test_cancel();
        std::cout << "  test_cancel: PASS\n";

        test_repeating();
        std::cout << "  test_repeating: PASS\n";

        test_completed_count();
        std::cout << "  test_completed_count: PASS\n";

        test_pending_tasks();
        std::cout << "  test_pending_tasks: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "task_scheduler_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\ntask_scheduler_tests passed" << std::endl;
    return 0;
}
