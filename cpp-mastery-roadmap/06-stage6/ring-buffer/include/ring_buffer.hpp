#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <stdexcept>

namespace ring_buffer {

template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(std::size_t capacity)
        : buffer_(capacity)
        , capacity_(capacity)
        , head_(0)
        , tail_(0)
        , size_(0) {}

    bool push(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (size_ == capacity_) {
            return false;  // Full
        }

        buffer_[tail_] = item;
        tail_ = (tail_ + 1) % capacity_;
        size_++;
        condition_.notify_one();
        return true;
    }

    bool push(T&& item) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (size_ == capacity_) {
            return false;  // Full
        }

        buffer_[tail_] = std::move(item);
        tail_ = (tail_ + 1) % capacity_;
        size_++;
        condition_.notify_one();
        return true;
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (size_ == 0) {
            return std::nullopt;  // Empty
        }

        T item = std::move(buffer_[head_]);
        head_ = (head_ + 1) % capacity_;
        size_--;
        return item;
    }

    std::optional<T> popWait(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
        std::unique_lock<std::mutex> lock(mutex_);

        condition_.wait_for(lock, timeout, [this]() {
            return size_ > 0;
        });

        if (size_ == 0) {
            return std::nullopt;
        }

        T item = std::move(buffer_[head_]);
        head_ = (head_ + 1) % capacity_;
        size_--;
        return item;
    }

    std::optional<T> peek() const {
        std::lock_guard<std::mutex> lock(mutex_);

        if (size_ == 0) {
            return std::nullopt;
        }

        return buffer_[head_];
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_ == 0;
    }

    bool full() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_ == capacity_;
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        head_ = 0;
        tail_ = 0;
        size_ = 0;
    }

private:
    std::vector<T> buffer_;
    std::size_t capacity_;
    std::size_t head_;
    std::size_t tail_;
    std::size_t size_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
};

} // namespace ring_buffer
