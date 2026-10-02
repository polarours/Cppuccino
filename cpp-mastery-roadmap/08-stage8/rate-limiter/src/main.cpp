#include "rate_limiter.hpp"

#include <iostream>
#include <thread>

int main() {
    using namespace std::chrono;

    rate_limiter::TokenBucket bucket(/*capacity=*/3, /*refillPerSecond=*/10.0);
    std::cout << "burst up to 3: ";
    for (int i = 0; i < 5; ++i)
        std::cout << (bucket.tryAcquire() ? 'Y' : 'N');
    std::cout << "\n";

    std::this_thread::sleep_for(milliseconds(150));  // ~1.5 tokens refill
    std::cout << "after 150ms, tokens=" << bucket.tokens()
              << ", try=" << (bucket.tryAcquire() ? "allowed" : "limited") << "\n";

    rate_limiter::FixedWindowLimiter window(3, seconds(1));
    std::cout << "fixed window (limit 3): ";
    for (int i = 0; i < 5; ++i)
        std::cout << (window.tryAcquire("client") ? 'Y' : 'N');
    std::cout << "\n";
    return 0;
}
