#pragma once

#include <vector>
#include <mutex>
#include <iostream>

namespace double_buffer {

template <typename T>
class DoubleBuffer {
public:
    explicit DoubleBuffer(std::size_t size)
        : front_(size), back_(size), size_(size) {}

    void write(const std::vector<T>& data) {
        std::lock_guard<std::mutex> lock(writeMutex_);
        for (std::size_t i = 0; i < data.size() && i < size_; ++i) {
            back_[i] = data[i];
        }
    }

    std::vector<T> read() {
        std::lock_guard<std::mutex> lock(readMutex_);
        return front_;
    }

    void swap() {
        std::lock_guard<std::mutex> writeLock(writeMutex_);
        std::lock_guard<std::mutex> readLock(readMutex_);
        front_.swap(back_);
    }

    std::size_t size() const { return size_; }

    void setBack(std::size_t index, const T& value) {
        std::lock_guard<std::mutex> lock(writeMutex_);
        if (index < size_) back_[index] = value;
    }

    T getFront(std::size_t index) const {
        std::lock_guard<std::mutex> lock(readMutex_);
        return (index < size_) ? front_[index] : T{};
    }

private:
    std::vector<T> front_;
    std::vector<T> back_;
    std::size_t size_;
    mutable std::mutex writeMutex_;
    mutable std::mutex readMutex_;
};

} // namespace double_buffer
