#include "task_scheduler.hpp"

#include <iostream>
#include <algorithm>

namespace task_scheduler {

TaskScheduler::TaskScheduler(std::size_t threadCount)
    : running_(false)
    , stopped_(false)
    , completedCount_(0)
    , nextId_(1) {
    for (std::size_t i = 0; i < threadCount; ++i) {
        workers_.emplace_back(&TaskScheduler::workerLoop, this);
    }
}

TaskScheduler::~TaskScheduler() {
    stop();
}

TaskId TaskScheduler::schedule(TaskFunc func, TaskPriority priority) {
    std::lock_guard<std::mutex> lock(mutex_);
    TaskId id = nextId_++;
    auto executeAt = std::chrono::steady_clock::now();
    taskQueue_.push({id, std::move(func), priority, executeAt, false, std::chrono::milliseconds(0)});
    activeTasks_[id] = true;
    condition_.notify_one();
    return id;
}

TaskId TaskScheduler::scheduleAt(TaskFunc func, std::chrono::steady_clock::time_point time,
                                 TaskPriority priority) {
    std::lock_guard<std::mutex> lock(mutex_);
    TaskId id = nextId_++;
    taskQueue_.push({id, std::move(func), priority, time, false, std::chrono::milliseconds(0)});
    activeTasks_[id] = true;
    condition_.notify_one();
    return id;
}

TaskId TaskScheduler::scheduleRepeating(TaskFunc func, std::chrono::milliseconds interval,
                                        TaskPriority priority) {
    std::lock_guard<std::mutex> lock(mutex_);
    TaskId id = nextId_++;
    auto executeAt = std::chrono::steady_clock::now() + interval;
    taskQueue_.push({id, std::move(func), priority, executeAt, true, interval});
    activeTasks_[id] = true;
    condition_.notify_one();
    return id;
}

void TaskScheduler::cancel(TaskId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    activeTasks_[id] = false;
}

void TaskScheduler::start() {
    running_ = true;
    stopped_ = false;
}

void TaskScheduler::stop() {
    stopped_ = true;
    running_ = false;
    condition_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

std::size_t TaskScheduler::pendingTasks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return taskQueue_.size();
}

std::size_t TaskScheduler::completedTasks() const {
    return completedCount_;
}

void TaskScheduler::workerLoop() {
    while (!stopped_) {
        Task task;
        bool hasTask = false;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait_for(lock, std::chrono::milliseconds(10), [this]() {
                return stopped_ || !taskQueue_.empty();
            });

            if (stopped_ && taskQueue_.empty()) {
                break;
            }

            if (!taskQueue_.empty()) {
                auto now = std::chrono::steady_clock::now();
                if (taskQueue_.top().executeAt <= now) {
                    task = taskQueue_.top();
                    taskQueue_.pop();
                    hasTask = true;
                }
            }
        }

        if (hasTask) {
            auto it = activeTasks_.find(task.id);
            if (it != activeTasks_.end() && it->second) {
                task.func();
                completedCount_++;

                if (task.recurring) {
                    std::lock_guard<std::mutex> lock(mutex_);
                    task.executeAt = std::chrono::steady_clock::now() + task.interval;
                    taskQueue_.push(task);
                }
            }
        }
    }
}

} // namespace task_scheduler
