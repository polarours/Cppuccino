#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <stdexcept>

namespace connection_pool {

template <typename T>
class ConnectionPool {
public:
    using Creator = std::function<T()>;
    using Deleter = std::function<void(T*)>;

    ConnectionPool(Creator creator, std::size_t minSize, std::size_t maxSize,
                   std::chrono::milliseconds timeout = std::chrono::milliseconds(5000))
        : creator_(std::move(creator))
        , minSize_(minSize)
        , maxSize_(maxSize)
        , timeout_(timeout)
        , closed_(false) {
        for (std::size_t i = 0; i < minSize; ++i) {
            pool_.push(creator_());
        }
    }

    ~ConnectionPool() {
        close();
    }

    T acquire() {
        std::unique_lock<std::mutex> lock(mutex_);

        if (closed_) {
            throw std::runtime_error("Pool is closed");
        }

        if (!pool_.empty()) {
            T conn = std::move(pool_.front());
            pool_.pop();
            return conn;
        }

        if (activeConnections_ < maxSize_) {
            activeConnections_++;
            return creator_();
        }

        condition_.wait_for(lock, timeout_, [this]() {
            return !pool_.empty() || closed_;
        });

        if (closed_) {
            throw std::runtime_error("Pool is closed");
        }

        if (pool_.empty()) {
            throw std::runtime_error("Connection pool timeout");
        }

        T conn = std::move(pool_.front());
        pool_.pop();
        return conn;
    }

    void release(T connection) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (closed_) {
            return;
        }

        pool_.push(std::move(connection));
        activeConnections_--;
        condition_.notify_one();
    }

    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
        condition_.notify_all();
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
    }

    std::size_t activeConnections() const {
        return activeConnections_;
    }

private:
    Creator creator_;
    std::size_t minSize_;
    std::size_t maxSize_;
    std::chrono::milliseconds timeout_;
    std::queue<T> pool_;
    std::atomic<std::size_t> activeConnections_{0};
    std::atomic<bool> closed_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
};

} // namespace connection_pool
