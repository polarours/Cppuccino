#include "command_queue_v2.hpp"

namespace command_queue_v2 {

CommandQueue::CommandQueue(std::size_t numWorkers)
    : shutdown_(false) {
    for (std::size_t i = 0; i < numWorkers; ++i) {
        workers_.emplace_back(&CommandQueue::workerLoop, this);
    }
}

CommandQueue::~CommandQueue() {
    shutdown();
}

void CommandQueue::enqueue(Task task, Priority priority) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.push({std::move(task), priority});
    }
    condition_.notify_one();
}

void CommandQueue::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutdown_ = true;
    }
    condition_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void CommandQueue::waitForAll() {
    std::unique_lock<std::mutex> lock(mutex_);
    finished_.wait(lock, [this]() {
        return tasks_.empty() && activeTasks_ == 0;
    });
}

std::size_t CommandQueue::pendingTasks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return tasks_.size();
}

std::size_t CommandQueue::completedTasks() const {
    return completedCount_;
}

bool CommandQueue::isShutdown() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return shutdown_;
}

void CommandQueue::workerLoop() {
    while (true) {
        PriorityTask task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]() {
                return shutdown_ || !tasks_.empty();
            });

            if (shutdown_ && tasks_.empty()) {
                return;
            }

            task = tasks_.top();
            tasks_.pop();
            activeTasks_++;
        }

        task.task();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            activeTasks_--;
            completedCount_++;
            if (activeTasks_ == 0 && tasks_.empty()) {
                finished_.notify_all();
            }
        }
    }
}

} // namespace command_queue_v2
