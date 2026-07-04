#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
#include <stdexcept>

namespace thread_pool {

class ThreadPool {
public:
    explicit ThreadPool(std::size_t numThreads);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    void shutdown();
    void waitForAll();

    std::size_t threadCount() const;
    std::size_t pendingTasks() const;
    bool isShutdown() const;

private:
    void workerLoop();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex mutex_;
    std::condition_variable condition_;
    std::condition_variable finished_;
    bool shutdown_;
    std::size_t activeTasks_;
};

// --- Template implementation ---

template <typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::invoke_result<F, Args...>::type>
{
    using return_type = typename std::invoke_result<F, Args...>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> result = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (shutdown_) {
            throw std::runtime_error("enqueue on shutdown ThreadPool");
        }
        tasks_.emplace([task]() { (*task)(); });
        activeTasks_++;
    }
    condition_.notify_one();
    return result;
}

} // namespace thread_pool
