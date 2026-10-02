#include "rate_limiter.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

using namespace std::chrono;
using Clock = rate_limiter::Clock;

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_token_bucket_burst_then_limit() {
    rate_limiter::TokenBucket b(3, 10.0);
    auto now = Clock::now();
    expect(b.tryAcquire(1, now), "1st");
    expect(b.tryAcquire(1, now), "2nd");
    expect(b.tryAcquire(1, now), "3rd (burst capacity)");
    expect(!b.tryAcquire(1, now), "4th limited");
}

void test_token_bucket_refills_over_time() {
    rate_limiter::TokenBucket b(2, 10.0);  // 10 tokens/sec -> 100ms per token
    auto now = Clock::now();
    expect(b.tryAcquire(1, now), "consume 1");
    expect(b.tryAcquire(1, now), "consume 2");
    expect(!b.tryAcquire(1, now), "empty");
    now += milliseconds(120);  // ~1.2 tokens
    expect(b.tryAcquire(1, now), "refilled after 120ms");
    expect(!b.tryAcquire(1, now), "only one token refilled");
    now += seconds(10);  // way past capacity
    expect(b.tryAcquire(1, now), "refill capped at capacity still allows 1");
    expect(b.tryAcquire(1, now), "capacity 2");
    expect(!b.tryAcquire(1, now), "no more than capacity");
}

void test_fixed_window_limit() {
    rate_limiter::FixedWindowLimiter w(2, seconds(1));
    auto now = Clock::now();
    expect(w.tryAcquire("a", now), "1");
    expect(w.tryAcquire("a", now), "2");
    expect(!w.tryAcquire("a", now), "3rd denied in same window");
    expect(w.observedCount("a", now) == 2, "count 2");
    // different key has its own window
    expect(w.tryAcquire("b", now), "other key unaffected");
    // new window after 1s
    now += seconds(1);
    expect(w.tryAcquire("a", now + seconds(1)), "new window allows again");
    expect(w.observedCount("a", now + seconds(1)) == 1, "count reset");
}

void test_sliding_window_drops_old_entries() {
    rate_limiter::SlidingWindowLimiter s(2, seconds(1));
    auto now = Clock::now();
    expect(s.tryAcquire("k", now), "1");
    expect(s.tryAcquire("k", now + milliseconds(100)), "2");
    expect(!s.tryAcquire("k", now + milliseconds(200)), "3rd denied (in window)");
    // 1.05s after t=0: first entry slid out (>=1s), but the t=100ms entry
    // is still inside the window -> exactly one slot freed.
    auto later = now + milliseconds(1050);
    expect(s.tryAcquire("k", later), "old entry slid out");
    expect(!s.tryAcquire("k", later), "t=100ms entry still occupies a slot");
}

void test_per_key_isolation() {
    rate_limiter::FixedWindowLimiter w(1, seconds(1));
    auto now = Clock::now();
    expect(w.tryAcquire("userA", now), "A allowed");
    expect(!w.tryAcquire("userA", now), "A denied");
    expect(w.tryAcquire("userB", now), "B independent");
}

void test_real_time_refill_smoke() {
    // Real-clock smoke: capacity 1, refill 5/sec -> one token per 200ms
    rate_limiter::TokenBucket b(1, 5.0);
    expect(b.tryAcquire(), "immediate");
    expect(!b.tryAcquire(), "second immediate denied");
    std::this_thread::sleep_for(milliseconds(250));
    expect(b.tryAcquire(), "refilled after 250ms");
}

}  // namespace

int main() {
    try {
        test_token_bucket_burst_then_limit();
        test_token_bucket_refills_over_time();
        test_fixed_window_limit();
        test_sliding_window_drops_old_entries();
        test_per_key_isolation();
        test_real_time_refill_smoke();
        std::cout << "All rate_limiter tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
