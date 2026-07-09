#pragma once

#include <string>
#include <vector>
#include <queue>
#include <functional>
#include <memory>
#include <chrono>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <map>

namespace task_scheduler {

using TaskId = std::size_t;
using TaskFunc = std::function<void()>;

enum class TaskPriority {
    Low = 0,
    Normal = 1,
    High = 2
};

struct Task {
    TaskId id;
    TaskFunc func;
    TaskPriority priority;
    std::chrono::steady_clock::time_point executeAt;
    bool recurring = false;
    std::chrono::milliseconds interval{0};

    bool operator>(const Task& other) const {
        if (priority != other.priority) {
            return priority < other.priority;
        }
        return executeAt > other.executeAt;
    }
};

class TaskScheduler {
public:
    TaskScheduler(std::size_t threadCount = 4);
    ~TaskScheduler();

    TaskId schedule(TaskFunc func, TaskPriority priority = TaskPriority::Normal);
    TaskId scheduleAt(TaskFunc func, std::chrono::steady_clock::time_point time,
                      TaskPriority priority = TaskPriority::Normal);
    TaskId scheduleRepeating(TaskFunc func, std::chrono::milliseconds interval,
                             TaskPriority priority = TaskPriority::Normal);

    void cancel(TaskId id);

    void start();
    void stop();

    std::size_t pendingTasks() const;
    std::size_t completedTasks() const;

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::priority_queue<Task, std::vector<Task>, std::greater<Task>> taskQueue_;
    std::map<TaskId, bool> activeTasks_;

    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::condition_variable finished_;

    std::atomic<bool> running_;
    std::atomic<bool> stopped_;
    std::atomic<std::size_t> completedCount_;
    TaskId nextId_;
};

} // namespace task_scheduler
