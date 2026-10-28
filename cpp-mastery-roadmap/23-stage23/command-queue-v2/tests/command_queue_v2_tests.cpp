#include "command_queue_v2.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

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

// The queue knows how many tasks are waiting and how many have run. A
// worker that is busy leaves the rest of the tasks in the queue, so the
// two counters tell them apart.
void test_the_task_counters() {
    command_queue_v2::CommandQueue queue(1);

    std::mutex gateMutex;
    std::condition_variable gate;
    bool released = false;

    // The first task occupies the only worker.
    queue.enqueue([&] {
        std::unique_lock<std::mutex> lock(gateMutex);
        gate.wait(lock, [&] { return released; });
    });
    // The other two cannot start, so they wait in the queue.
    queue.enqueue([] {});
    queue.enqueue([] {});

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    expect(queue.pendingTasks() == 2, "expected 2 tasks waiting");
    expect(queue.completedTasks() == 0, "expected nothing completed yet");

    {
        std::lock_guard<std::mutex> lock(gateMutex);
        released = true;
    }
    gate.notify_all();

    queue.waitForAll();
    expect(queue.pendingTasks() == 0, "expected nothing waiting");
    expect(queue.completedTasks() == 3, "expected 3 completed tasks");
}

// shutdown() stops the workers, and the queue says that it has.
void test_shutdown() {
    command_queue_v2::CommandQueue queue(2);
    std::atomic<int> counter{0};

    expect(!queue.isShutdown(), "expected the queue to be running");

    queue.enqueue([&counter]() { counter++; });
    queue.waitForAll();
    expect(counter == 1, "expected 1 task");

    queue.shutdown();
    expect(queue.isShutdown(), "expected the queue to be shut down");
    expect(queue.completedTasks() == 1, "expected 1 completed task");
}

// The tasks that are already in the queue when the queue is shut down are
// still run: shutting down stops the workers, it does not throw the work
// away.
void test_shutdown_runs_the_tasks_that_are_waiting() {
    command_queue_v2::CommandQueue queue(1);

    std::mutex gateMutex;
    std::condition_variable gate;
    bool released = false;
    std::atomic<int> counter{0};

    queue.enqueue([&] {
        std::unique_lock<std::mutex> lock(gateMutex);
        gate.wait(lock, [&] { return released; });
    });
    queue.enqueue([&counter]() { counter++; });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    {
        std::lock_guard<std::mutex> lock(gateMutex);
        released = true;
    }
    gate.notify_all();

    queue.shutdown();
    expect(counter == 1, "expected the waiting task to have run");
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

        test_the_task_counters();
        std::cout << "  test_the_task_counters: PASS\n";

        test_shutdown();
        std::cout << "  test_shutdown: PASS\n";

        test_shutdown_runs_the_tasks_that_are_waiting();
        std::cout << "  test_shutdown_runs_the_tasks_that_are_waiting: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "command_queue_v2_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\ncommand_queue_v2_tests passed" << std::endl;
    return 0;
}
