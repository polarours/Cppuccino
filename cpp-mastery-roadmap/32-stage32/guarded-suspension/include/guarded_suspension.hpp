#pragma once

#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <atomic>
#include <iostream>

namespace guarded_suspension {

class GuardedObject {
public:
    using Predicate = std::function<bool()>;

    void set(int value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_.store(value);
        ready_.store(true);
        condition_.notify_all();
    }

    bool waitFor(Predicate pred, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
        std::unique_lock<std::mutex> lock(mutex_);
        // The predicate is called while the lock is held, because that is
        // what a condition variable asks for. The state it looks at is
        // atomic, so a predicate that asks the object about itself --
        // waitFor([&] { return obj.isReady(); }) -- does not wait for a
        // lock this thread already holds.
        return condition_.wait_for(lock, timeout, [this, &pred] {
            return pred();
        });
    }

    // The state is atomic, so reading it needs no lock. That is what
    // lets a predicate of waitFor() look at the object without
    // deadlocking.
    int get() const {
        return value_.load();
    }

    bool isReady() const {
        return ready_.load();
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        ready_.store(false);
        value_.store(0);
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    // Only the writers take the mutex. The state itself is atomic, so
    // that a reader -- a predicate of waitFor(), for instance -- never
    // has to wait for the lock.
    std::atomic<int> value_{0};
    std::atomic<bool> ready_{false};
};

} // namespace guarded_suspension
