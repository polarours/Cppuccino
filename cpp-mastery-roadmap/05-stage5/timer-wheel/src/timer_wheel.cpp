#include "timer_wheel.hpp"

#include <algorithm>

namespace timer_wheel {

std::uint64_t TimerWheel::scheduleAt(std::uint64_t fireAtTickAbs, Callback cb) {
    // Past-due timers fire on the very next tick.
    if (fireAtTickAbs <= currentTick_) fireAtTickAbs = currentTick_ + 1;
    const std::uint64_t id = nextId_++;
    buckets_[slotFor(fireAtTickAbs)].push_back(Entry{id, fireAtTickAbs, std::move(cb)});
    return id;
}

bool TimerWheel::cancel(std::uint64_t id) {
    for (auto& bucket : buckets_) {
        auto it = std::find_if(bucket.begin(), bucket.end(),
                               [id](const Entry& e) { return e.id == id; });
        if (it != bucket.end()) {
            bucket.erase(it);
            return true;
        }
    }
    return false;
}

std::size_t TimerWheel::tick(std::size_t n) {
    std::size_t fired = 0;
    for (std::size_t step = 0; step < n; ++step) {
        // Advance time FIRST, then fire everything due at the new tick.
        // (A timer scheduled at abs tick T fires when tick() brings us to T.)
        ++currentTick_;
        auto& bucket = buckets_[currentTick_ % slots_];
        std::vector<Entry> due;
        due.swap(bucket);
        for (auto& e : due) {
            if (e.fireAtAbs <= currentTick_) {
                if (e.cb) e.cb();
                ++fired;
            } else {
                // Far-future entry only collided by slot; put it back.
                buckets_[slotFor(e.fireAtAbs)].push_back(std::move(e));
            }
        }
    }
    return fired;
}

std::size_t TimerWheel::pendingCount() const {
    std::size_t total = 0;
    for (const auto& b : buckets_) total += b.size();
    return total;
}

}  // namespace timer_wheel
