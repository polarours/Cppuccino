#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <atomic>

namespace command_queue_v2 {

using Task = std::function<void()>;

enum class Priority { Low = 0, Normal = 1, High = 2 };

struct PriorityTask {
    Task task;
    Priority priority;

    bool operator>(const PriorityTask& other) const {
        return priority < other.priority;
    }
};

class CommandQueue {
public:
    explicit CommandQueue(std::size_t numWorkers = 4);
    ~CommandQueue();

    void enqueue(Task task, Priority priority = Priority::Normal);
    void shutdown();
    void waitForAll();

    std::size_t pendingTasks() const;
    std::size_t completedTasks() const;
    bool isShutdown() const;

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::priority_queue<PriorityTask, std::vector<PriorityTask>, std::greater<PriorityTask>> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::condition_variable finished_;
    bool shutdown_;
    std::atomic<std::size_t> activeTasks_{0};
    std::atomic<std::size_t> completedCount_{0};
};

} // namespace command_queue_v2
