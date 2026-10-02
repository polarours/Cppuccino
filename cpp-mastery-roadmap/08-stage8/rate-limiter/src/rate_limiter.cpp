#include "rate_limiter.hpp"

namespace rate_limiter {

// ---- TokenBucket ----
void TokenBucket::refill(Clock::time_point now) {
    if (now <= last_) return;
    double elapsed =
        std::chrono::duration<double>(now - last_).count();
    tokens_ += elapsed * refillPerSec_;
    if (tokens_ > static_cast<double>(capacity_))
        tokens_ = static_cast<double>(capacity_);
    last_ = now;
}

bool TokenBucket::tryAcquire(std::size_t n, Clock::time_point now) {
    std::lock_guard<std::mutex> lock(m_);
    refill(now);
    if (tokens_ >= static_cast<double>(n)) {
        tokens_ -= static_cast<double>(n);
        return true;
    }
    return false;
}

double TokenBucket::tokens(Clock::time_point now) {
    std::lock_guard<std::mutex> lock(m_);
    refill(now);
    return tokens_;
}

// ---- FixedWindowLimiter ----
bool FixedWindowLimiter::tryAcquire(const std::string& key, Clock::time_point now) {
    std::lock_guard<std::mutex> lock(m_);
    auto it = windows_.find(key);
    if (it == windows_.end() || now - it->second.start >= window_) {
        windows_[key] = Window{now, 1};
        return true;
    }
    if (it->second.count < limit_) {
        ++it->second.count;
        return true;
    }
    return false;
}

std::size_t FixedWindowLimiter::observedCount(const std::string& key,
                                              Clock::time_point now) const {
    std::lock_guard<std::mutex> lock(m_);
    auto it = windows_.find(key);
    if (it == windows_.end() || now - it->second.start >= window_) return 0;
    return it->second.count;
}

// ---- SlidingWindowLimiter ----
bool SlidingWindowLimiter::tryAcquire(const std::string& key, Clock::time_point now) {
    std::lock_guard<std::mutex> lock(m_);
    auto& log = logs_[key];
    // drop timestamps outside the window
    while (!log.empty() && now - log.front() >= window_) log.pop_front();
    if (log.size() >= limit_) return false;
    log.push_back(now);
    return true;
}

}  // namespace rate_limiter
