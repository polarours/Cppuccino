#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <iostream>

namespace balking_pattern {

class Document {
public:
    void open() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (opened_) return;
        opened_ = true;
    }

    void save() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;
        saved_ = true;
    }

    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;
        opened_ = false;
        saved_ = false;
    }

    // The state is atomic so isOpen()/isSaved() can be read from another
    // thread; the mutex keeps the transitions themselves exclusive.
    bool isOpen() const { return opened_.load(); }
    bool isSaved() const { return saved_.load(); }

private:
    std::mutex mutex_;
    std::atomic<bool> opened_{false};
    std::atomic<bool> saved_{false};
};

class Sensor {
public:
    explicit Sensor(int threshold = 100) : threshold_(threshold) {}

    bool read(int value) {
        if (value > threshold_) {
            lastValue_ = value;
            triggered_ = true;
            return true;
        }
        return false;
    }

    bool wasTriggered() const { return triggered_; }
    int lastValue() const { return lastValue_; }

private:
    int threshold_;
    std::atomic<bool> triggered_{false};
    std::atomic<int> lastValue_{0};
};

} // namespace balking_pattern
