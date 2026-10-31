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

    // The worker list is taken under the lock, because threadCount()
    // reads it and the two must not touch the same vector at the same
    // time. The join happens with the list in a local variable and
    // without the lock held: a worker that is still running needs the
    // lock to finish.
    std::vector<std::thread> workers;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        workers.swap(workers_);
    }

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::waitForAll() {
    std::unique_lock<std::mutex> lock(mutex_);
    finished_.wait(lock, [this]() {
        return activeTasks_ == 0 && tasks_.empty();
    });
}

std::size_t ThreadPool::threadCount() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return workers_.size();
}

std::size_t ThreadPool::pendingTasks() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return tasks_.size();
}

bool ThreadPool::isShutdown() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return shutdown_;
}

} // namespace thread_pool
