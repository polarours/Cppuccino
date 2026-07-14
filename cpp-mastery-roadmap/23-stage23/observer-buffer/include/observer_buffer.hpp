#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <functional>

namespace observer_buffer {

struct Event {
    std::string type;
    std::string data;
};

using Handler = std::function<void(const Event&)>;

class BufferedDispatcher {
public:
    explicit BufferedDispatcher(std::size_t bufferSize = 100)
        : bufferSize_(bufferSize) {}

    void setHandler(Handler handler) {
        handler_ = handler;
    }

    void emit(const Event& event) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            buffer_.push_back(event);
        }

        if (bufferSize() >= bufferSize_) {
            flush();
        }
    }

    void flush() {
        std::vector<Event> events;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            events.swap(buffer_);
        }
        for (const auto& event : events) {
            if (handler_) {
                handler_(event);
            }
        }
    }

    std::size_t bufferSize() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_.size();
    }

private:
    std::size_t bufferSize_;
    Handler handler_;
    std::vector<Event> buffer_;
    mutable std::mutex mutex_;
};

} // namespace observer_buffer
