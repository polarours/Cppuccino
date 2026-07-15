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

    bool isOpen() const { return opened_; }
    bool isSaved() const { return saved_; }

private:
    std::mutex mutex_;
    bool opened_ = false;
    bool saved_ = false;
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
