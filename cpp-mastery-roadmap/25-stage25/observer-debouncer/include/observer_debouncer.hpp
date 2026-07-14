#pragma once

#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

namespace observer_debouncer {

struct Event {
    std::string type;
    std::string data;
};

using Handler = std::function<void(const Event&)>;

class DebouncedDispatcher {
public:
    explicit DebouncedDispatcher(std::chrono::milliseconds delay)
        : delay_(delay) {}

    ~DebouncedDispatcher() { cancel(); }

    void setHandler(Handler handler) {
        handler_ = std::move(handler);
    }

    void emit(const Event& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_ = event;

        if (timer_.joinable()) {
            timer_.detach();
        }

        timer_ = std::thread([this, event]() {
            std::this_thread::sleep_for(delay_);
            std::lock_guard<std::mutex> lock(mutex_);
            if (handler_) {
                handler_(event);
            }
            emitCount_++;
        });
    }

    void cancel() {
        if (timer_.joinable()) {
            timer_.detach();
        }
    }

    std::size_t emitCount() const { return emitCount_; }

private:
    std::chrono::milliseconds delay_;
    Handler handler_;
    Event pending_;
    std::thread timer_;
    mutable std::mutex mutex_;
    std::atomic<std::size_t> emitCount_{0};
};

} // namespace observer_debouncer
