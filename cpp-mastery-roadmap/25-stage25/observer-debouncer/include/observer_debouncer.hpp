#pragma once

#include <string>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <vector>
#include <iostream>

namespace observer_debouncer {

struct Event {
    std::string type;
    std::string data;
};

using Handler = std::function<void(const Event&)>;

// Delivers an event to the handler after a delay. Every emit is delivered
// on its own; the delay is how long the dispatcher waits before it calls
// the handler.
class DebouncedDispatcher {
public:
    explicit DebouncedDispatcher(std::chrono::milliseconds delay)
        : delay_(delay) {}

    // The dispatcher waits for the dispatches that are still on their way.
    // A thread that outlived the dispatcher would call the handler on an
    // object that no longer exists.
    ~DebouncedDispatcher() {
        cancel();
        std::vector<std::shared_ptr<Timer>> timers;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            timers.swap(timers_);
        }
        for (auto& timer : timers) {
            if (timer->thread.joinable()) {
                timer->thread.join();
            }
        }
    }

    void setHandler(Handler handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        handler_ = std::move(handler);
    }

    void emit(const Event& event) {
        // The dispatches that have already run are joined here, so that the
        // list of threads does not grow without bound.
        reap();

        auto timer = std::make_shared<Timer>();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pending_ = event;
            // A new dispatch undoes an earlier cancel().
            cancelled_ = false;
        }

        timer->thread = std::thread([this, event, timer]() {
            std::this_thread::sleep_for(delay_);
            std::lock_guard<std::mutex> lock(mutex_);
            // A dispatch that was cancelled does not reach the handler.
            if (!cancelled_) {
                if (handler_) {
                    handler_(event);
                }
                emitCount_++;
            }
            timer->finished = true;
        });

        std::lock_guard<std::mutex> lock(mutex_);
        timers_.push_back(timer);
    }

    // Stops the dispatches that have not happened yet.
    void cancel() {
        std::lock_guard<std::mutex> lock(mutex_);
        cancelled_ = true;
    }

    std::size_t emitCount() const { return emitCount_.load(); }

private:
    struct Timer {
        std::thread thread;
        // Guarded by mutex_.
        bool finished = false;
    };

    // Joins the dispatches that have already run.
    void reap() {
        std::vector<std::shared_ptr<Timer>> done;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto it = timers_.begin(); it != timers_.end();) {
                if ((*it)->finished) {
                    done.push_back(std::move(*it));
                    it = timers_.erase(it);
                } else {
                    ++it;
                }
            }
        }
        for (auto& timer : done) {
            if (timer->thread.joinable()) {
                timer->thread.join();
            }
        }
    }

    std::chrono::milliseconds delay_;
    Handler handler_;
    Event pending_;
    std::vector<std::shared_ptr<Timer>> timers_;
    bool cancelled_ = false;
    mutable std::mutex mutex_;
    std::atomic<std::size_t> emitCount_{0};
};

}  // namespace observer_debouncer
