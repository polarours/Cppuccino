#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>

namespace rate_limiter {

using Clock = std::chrono::steady_clock;

// Token bucket: capacity = burst size, refillPerSecond = sustained rate.
class TokenBucket {
public:
    TokenBucket(std::size_t capacity, double refillPerSecond)
        : capacity_(capacity), tokens_(static_cast<double>(capacity)),
          refillPerSec_(refillPerSecond) {}

    // Try to consume one token; false when rate-limited.
    bool tryAcquire(std::size_t n = 1, Clock::time_point now = Clock::now());

    double tokens(Clock::time_point now = Clock::now());

private:
    void refill(Clock::time_point now);

    const std::size_t capacity_;
    const double refillPerSec_;
    double tokens_;
    Clock::time_point last_ = Clock::now();
    mutable std::mutex m_;
};

// Fixed window counter: allows `limit` requests per window (deterministic
// under a manually-advanced clock - ideal for tests).
class FixedWindowLimiter {
public:
    FixedWindowLimiter(std::size_t limit, std::chrono::milliseconds window)
        : limit_(limit), window_(window) {}

    bool tryAcquire(const std::string& key,
                    Clock::time_point now = Clock::now());

    std::size_t observedCount(const std::string& key,
                              Clock::time_point now = Clock::now()) const;

private:
    struct Window {
        Clock::time_point start;
        std::size_t count = 0;
    };

    const std::size_t limit_;
    const std::chrono::milliseconds window_;
    mutable std::mutex m_;
    std::unordered_map<std::string, Window> windows_;
};

// Sliding window log: keeps timestamps of the last `limit` requests.
class SlidingWindowLimiter {
public:
    explicit SlidingWindowLimiter(std::size_t limit,
                                  std::chrono::milliseconds window)
        : limit_(limit), window_(window) {}

    bool tryAcquire(const std::string& key,
                    Clock::time_point now = Clock::now());

private:
    const std::size_t limit_;
    const std::chrono::milliseconds window_;
    mutable std::mutex m_;
    std::unordered_map<std::string, std::deque<Clock::time_point>> logs_;
};

}  // namespace rate_limiter
