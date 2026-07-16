#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <iostream>

namespace producer_consumer {

template <typename T>
class BoundedQueue {
public:
    explicit BoundedQueue(std::size_t capacity) : capacity_(capacity) {}

    bool push(const T& item, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!notFull_.wait_for(lock, timeout, [this] { return queue_.size() < capacity_; })) {
            return false;
        }
        queue_.push(item);
        notEmpty_.notify_one();
        return true;
    }

    bool pop(T& item, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!notEmpty_.wait_for(lock, timeout, [this] { return !queue_.empty(); })) {
            return false;
        }
        item = queue_.front();
        queue_.pop();
        notFull_.notify_one();
        return true;
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::size_t capacity_;
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable notFull_;
    std::condition_variable notEmpty_;
};

} // namespace producer_consumer
