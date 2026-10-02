#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace timer_wheel {

// Hierarchical-free single-level timing wheel: one slot per tick.
// Callbacks are scheduled by absolute tick; tick() advances time and fires
// everything due. Rescheduling happens by re-insert (one-shot semantics).
class TimerWheel {
public:
    using Callback = std::function<void()>;

    // slots: wheel size; tickIntervalMs: real ms each tick covers.
    explicit TimerWheel(std::size_t slots = 64, std::uint64_t tickIntervalMs = 10)
        : slots_(slots), interval_(tickIntervalMs),
          buckets_(slots) {
        if (slots_ == 0) slots_ = 1;
    }

    // Schedule fireAtTickAbs -> returns a timer id.
    std::uint64_t scheduleAt(std::uint64_t fireAtTickAbs, Callback cb);

    // Schedule relative to current tick position.
    std::uint64_t scheduleIn(std::uint64_t ticksFromNow, Callback cb) {
        return scheduleAt(currentTick_ + ticksFromNow, std::move(cb));
    }

    // Cancel a pending timer; true if it was still pending.
    bool cancel(std::uint64_t id);

    // Advance the wheel by n ticks, firing due callbacks.
    // Returns how many callbacks fired.
    std::size_t tick(std::size_t n = 1);

    std::uint64_t currentTick() const { return currentTick_; }
    std::size_t pendingCount() const;

    std::uint64_t intervalMs() const { return interval_; }

private:
    struct Entry {
        std::uint64_t id;
        std::uint64_t fireAtAbs;
        Callback cb;
    };

    std::size_t slotFor(std::uint64_t fireAtAbs) const {
        return fireAtAbs % slots_;
    }

    std::size_t slots_;
    std::uint64_t interval_;
    std::uint64_t currentTick_ = 0;
    std::uint64_t nextId_ = 1;
    std::vector<std::vector<Entry>> buckets_;
};

}  // namespace timer_wheel
