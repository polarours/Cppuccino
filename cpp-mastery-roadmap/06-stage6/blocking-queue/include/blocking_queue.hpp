#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>

namespace blocking_queue {

// Bounded multi-producer multi-consumer queue.
// push blocks when full (or times out), pop blocks when empty.
template <typename T>
class BlockingQueue {
public:
    explicit BlockingQueue(std::size_t capacity) : capacity_(capacity) {}

    // Blocks until space is available; false if closed or timeout.
    // Default timeout = infinite (never wait_for with max(): now()+max()
    // overflows the steady_clock deadline and degrades to a non-wait).
    bool push(T value,
              std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
        std::unique_lock<std::mutex> lock(m_);
        auto hasSpace = [&] { return queue_.size() < capacity_ || closed_; };
        if (timeout == std::chrono::milliseconds::max()) {
            cvNotFull_.wait(lock, hasSpace);  // unbounded, pred guaranteed true
        } else if (!cvNotFull_.wait_for(lock, timeout, hasSpace)) {
            return false;  // timeout
        }
        if (closed_) return false;
        queue_.push_back(std::move(value));
        cvNotEmpty_.notify_one();
        return true;
    }

    // Blocks until an item is available; nullopt if closed or timeout.
    std::optional<T> pop(
        std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) {
        std::unique_lock<std::mutex> lock(m_);
        auto hasItem = [&] { return !queue_.empty() || closed_; };
        if (timeout == std::chrono::milliseconds::max()) {
            cvNotEmpty_.wait(lock, hasItem);  // unbounded (no deadline overflow)
        } else if (!cvNotEmpty_.wait_for(lock, timeout, hasItem)) {
            return std::nullopt;  // timeout
        }
        if (queue_.empty()) return std::nullopt;  // closed and drained
        T value = std::move(queue_.front());
        queue_.pop_front();
        cvNotFull_.notify_one();
        return value;
    }

    // Close: wake all waiters; subsequent push fails, pop drains then nullopt.
    void close() {
        std::lock_guard<std::mutex> lock(m_);
        closed_ = true;
        cvNotEmpty_.notify_all();
        cvNotFull_.notify_all();
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(m_);
        return queue_.size();
    }
    bool closed() const {
        std::lock_guard<std::mutex> lock(m_);
        return closed_;
    }
    std::size_t capacity() const { return capacity_; }

private:
    const std::size_t capacity_;
    mutable std::mutex m_;
    std::condition_variable cvNotEmpty_;
    std::condition_variable cvNotFull_;
    std::deque<T> queue_;
    bool closed_ = false;
};

}  // namespace blocking_queue
