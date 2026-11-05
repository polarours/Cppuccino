#include "event_loop.hpp"

#include <iostream>
#include <thread>

namespace event_loop {

EventLoop::EventLoop()
    : nextTimerId_(1)
    , running_(false)
    , stopped_(false) {}

EventLoop::~EventLoop() {
    stop();
}

void EventLoop::run() {
    running_ = true;
    stopped_ = false;

    while (!stopped_) {
        // Take one task at a time. The callback runs with the mutex
        // released, so it may post more work or cancel a timer.
        Callback task;
        bool hasTask = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!taskQueue_.empty()) {
                task = std::move(taskQueue_.front());
                taskQueue_.pop();
                hasTask = true;
            }
        }
        if (hasTask) {
            task();
        }

        // Process timers
        processTimers();

        // Sleep briefly to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Check if we should stop
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (taskQueue_.empty() && timers_.empty()) {
                running_ = false;
            }
        }
    }

    running_ = false;
}

void EventLoop::stop() {
    stopped_ = true;
    running_ = false;
}

void EventLoop::post(Callback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    taskQueue_.push(std::move(callback));
}

TimerId EventLoop::schedule(Callback callback, std::chrono::milliseconds delay) {
    auto expiry = std::chrono::steady_clock::now() + delay;

    std::lock_guard<std::mutex> lock(mutex_);
    TimerId id = nextTimerId_++;
    timers_.push({id, expiry, std::move(callback), std::chrono::milliseconds(0), false});
    activeTimers_[id] = true;

    return id;
}

TimerId EventLoop::scheduleRepeating(Callback callback, std::chrono::milliseconds interval) {
    auto expiry = std::chrono::steady_clock::now() + interval;

    std::lock_guard<std::mutex> lock(mutex_);
    TimerId id = nextTimerId_++;
    timers_.push({id, expiry, std::move(callback), interval, true});
    activeTimers_[id] = true;

    return id;
}

void EventLoop::cancelTimer(TimerId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = activeTimers_.find(id);
    if (it != activeTimers_.end()) {
        it->second = false;
    }
}

bool EventLoop::isRunning() const {
    return running_;
}

bool EventLoop::hasPendingTasks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !taskQueue_.empty() || !timers_.empty();
}

void EventLoop::processTimers() {
    auto now = std::chrono::steady_clock::now();

    // The timers that are due are taken out under the mutex, and their
    // callbacks run with it released.
    std::vector<TimerEntry> due;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!timers_.empty() && timers_.top().expiry <= now) {
            TimerEntry entry = timers_.top();
            timers_.pop();

            // Check if timer is still active
            auto it = activeTimers_.find(entry.id);
            if (it != activeTimers_.end() && it->second) {
                if (entry.repeating) {
                    // Reschedule repeating timer
                    entry.expiry = now + entry.interval;
                    timers_.push(entry);
                } else {
                    activeTimers_.erase(it);
                }
                due.push_back(std::move(entry));
            } else {
                activeTimers_.erase(it);
            }
        }
    }

    for (auto& entry : due) {
        entry.callback();
    }
}

} // namespace event_loop
