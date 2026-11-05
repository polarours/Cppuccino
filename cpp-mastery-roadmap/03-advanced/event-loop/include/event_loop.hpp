#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <map>
#include <memory>
#include <chrono>
#include <atomic>
#include <mutex>

namespace event_loop {

using TimerId = std::size_t;
using Callback = std::function<void()>;

class EventLoop {
public:
    EventLoop();
    ~EventLoop();

    void run();
    void stop();

    void post(Callback callback);
    TimerId schedule(Callback callback, std::chrono::milliseconds delay);
    TimerId scheduleRepeating(Callback callback, std::chrono::milliseconds interval);

    void cancelTimer(TimerId id);

    bool isRunning() const;
    bool hasPendingTasks() const;

private:
    struct TimerEntry {
        TimerId id;
        std::chrono::steady_clock::time_point expiry;
        Callback callback;
        std::chrono::milliseconds interval{0};
        bool repeating = false;

        bool operator>(const TimerEntry& other) const {
            return expiry > other.expiry;
        }
    };

    void processTimers();

    // The queue, the timers, the cancellation flags and the timer counter
    // are all guarded by this mutex. post(), schedule() and cancelTimer()
    // are meant to be called from another thread while run() is going, and
    // the flags are atomic for the same reason -- but a flag that says
    // "there is work" is worth nothing if the work itself is not
    // protected.
    //
    // The callbacks run with the mutex released: a callback is free to
    // post more work or to cancel a timer, and both of those need the
    // mutex.
    mutable std::mutex mutex_;
    std::queue<Callback> taskQueue_;
    std::priority_queue<TimerEntry, std::vector<TimerEntry>, std::greater<TimerEntry>> timers_;
    std::map<TimerId, bool> activeTimers_;
    TimerId nextTimerId_;
    std::atomic<bool> running_;
    std::atomic<bool> stopped_;
};

} // namespace event_loop
