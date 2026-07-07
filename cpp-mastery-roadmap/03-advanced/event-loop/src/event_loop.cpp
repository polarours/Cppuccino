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
        // Process one task at a time
        if (!taskQueue_.empty()) {
            Callback task = std::move(taskQueue_.front());
            taskQueue_.pop();
            task();
        }

        // Process timers
        processTimers();

        // Sleep briefly to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Check if we should stop
        if (taskQueue_.empty() && timers_.empty()) {
            running_ = false;
        }
    }

    running_ = false;
}

void EventLoop::stop() {
    stopped_ = true;
    running_ = false;
}

void EventLoop::post(Callback callback) {
    taskQueue_.push(std::move(callback));
}

TimerId EventLoop::schedule(Callback callback, std::chrono::milliseconds delay) {
    TimerId id = nextTimerId_++;
    auto expiry = std::chrono::steady_clock::now() + delay;

    timers_.push({id, expiry, std::move(callback), std::chrono::milliseconds(0), false});
    activeTimers_[id] = true;

    return id;
}

TimerId EventLoop::scheduleRepeating(Callback callback, std::chrono::milliseconds interval) {
    TimerId id = nextTimerId_++;
    auto expiry = std::chrono::steady_clock::now() + interval;

    timers_.push({id, expiry, std::move(callback), interval, true});
    activeTimers_[id] = true;

    return id;
}

void EventLoop::cancelTimer(TimerId id) {
    auto it = activeTimers_.find(id);
    if (it != activeTimers_.end()) {
        it->second = false;
    }
}

bool EventLoop::isRunning() const {
    return running_;
}

bool EventLoop::hasPendingTasks() const {
    return !taskQueue_.empty() || !timers_.empty();
}

void EventLoop::processTimers() {
    auto now = std::chrono::steady_clock::now();

    while (!timers_.empty() && timers_.top().expiry <= now) {
        TimerEntry entry = timers_.top();
        timers_.pop();

        // Check if timer is still active
        auto it = activeTimers_.find(entry.id);
        if (it != activeTimers_.end() && it->second) {
            entry.callback();

            if (entry.repeating) {
                // Reschedule repeating timer
                entry.expiry = now + entry.interval;
                timers_.push(entry);
            } else {
                activeTimers_.erase(it);
            }
        } else {
            activeTimers_.erase(it);
        }
    }
}

} // namespace event_loop
