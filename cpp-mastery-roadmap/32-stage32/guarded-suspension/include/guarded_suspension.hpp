#pragma once

#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <iostream>

namespace guarded_suspension {

class GuardedObject {
public:
    using Predicate = std::function<bool()>;

    void set(int value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = value;
        ready_ = true;
        condition_.notify_all();
    }

    bool waitFor(Predicate pred, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
        std::unique_lock<std::mutex> lock(mutex_);
        return condition_.wait_for(lock, timeout, [this, &pred] {
            return pred();
        });
    }

    int get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_;
    }

    bool isReady() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return ready_;
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        ready_ = false;
        value_ = 0;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    int value_ = 0;
    bool ready_ = false;
};

} // namespace guarded_suspension
