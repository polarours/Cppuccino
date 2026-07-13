#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>

namespace command_queue {

using Task = std::function<void()>;

class CommandQueue {
public:
    explicit CommandQueue(std::size_t numWorkers = 4) {
        for (std::size_t i = 0; i < numWorkers; ++i) {
            workers_.emplace_back(&CommandQueue::workerLoop, this);
        }
    }

    ~CommandQueue() {
        shutdown();
    }

    void enqueue(Task task) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push(std::move(task));
        }
        condition_.notify_one();
    }

    void shutdown() {
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

    void waitForAll() {
        std::unique_lock<std::mutex> lock(mutex_);
        finished_.wait(lock, [this]() {
            return tasks_.empty() && activeTasks_ == 0;
        });
    }

    std::size_t pendingTasks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.size();
    }

    std::size_t completedTasks() const {
        return completedCount_;
    }

    bool isShutdown() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return shutdown_;
    }

private:
    void workerLoop() {
        while (true) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock, [this]() {
                    return shutdown_ || !tasks_.empty();
                });

                if (shutdown_ && tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
                activeTasks_++;
            }

            task();

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

    std::vector<std::thread> workers_;
    std::queue<Task> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::condition_variable finished_;
    bool shutdown_ = false;
    std::atomic<std::size_t> activeTasks_{0};
    std::atomic<std::size_t> completedCount_{0};
};

} // namespace command_queue
