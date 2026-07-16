#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <iostream>

namespace thread_pool_advanced {

class ThreadPool {
public:
    explicit ThreadPool(std::size_t numThreads) : stop_(false) {
        for (std::size_t i = 0; i < numThreads; ++i) {
            workers_.emplace_back([this] { workerLoop(); });
        }
    }

    ~ThreadPool() { shutdown(); }

    template <typename F>
    auto submit(F&& func) -> std::future<decltype(func())> {
        using RetType = decltype(func());
        auto task = std::make_shared<std::packaged_task<RetType()>>(std::forward<F>(func));
        auto future = task->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.emplace([task]() { (*task)(); });
        }
        condition_.notify_one();
        return future;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (auto& worker : workers_) {
            if (worker.joinable()) worker.join();
        }
    }

    std::size_t threadCount() const { return workers_.size(); }

    std::size_t pendingTasks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.size();
    }

private:
    void workerLoop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                if (stop_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool stop_;
};

} // namespace thread_pool_advanced
