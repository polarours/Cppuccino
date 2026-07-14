#pragma once

#include <string>
#include <functional>
#include <chrono>
#include <iostream>

namespace observer_throttler {

struct Event {
    std::string type;
    std::string data;
};

using Handler = std::function<void(const Event&)>;

class ThrottledDispatcher {
public:
    explicit ThrottledDispatcher(std::chrono::milliseconds interval)
        : interval_(interval) {}

    void setHandler(Handler handler) {
        handler_ = handler;
    }

    void emit(const Event& event) {
        auto now = std::chrono::steady_clock::now();
        if (now - lastEmit_ >= interval_) {
            if (handler_) {
                handler_(event);
            }
            lastEmit_ = now;
        }
    }

private:
    std::chrono::milliseconds interval_;
    Handler handler_;
    std::chrono::steady_clock::time_point lastEmit_;
};

} // namespace observer_throttler
