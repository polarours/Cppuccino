#include "thread_pool.hpp"

namespace thread_pool {

ThreadPool::ThreadPool(std::size_t numThreads)
    : shutdown_(false)
    , activeTasks_(0)
{
    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
    }

    for (std::size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back(&ThreadPool::workerLoop, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;
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
        }

        task();

        {
            std::unique_lock<std::mutex> lock(mutex_);
            activeTasks_--;
            if (activeTasks_ == 0 && tasks_.empty()) {
                finished_.notify_all();
            }
        }
    }
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        shutdown_ = true;
    }
    condition_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers_.clear();
}

void ThreadPool::waitForAll() {
    std::unique_lock<std::mutex> lock(mutex_);
    finished_.wait(lock, [this]() {
        return activeTasks_ == 0 && tasks_.empty();
    });
}

std::size_t ThreadPool::threadCount() const {
    return workers_.size();
}

std::size_t ThreadPool::pendingTasks() const {
    std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
    return tasks_.size();
}

bool ThreadPool::isShutdown() const {
    std::unique_lock<std::mutex> lock(const_cast<std::mutex&>(mutex_));
    return shutdown_;
}

} // namespace thread_pool
