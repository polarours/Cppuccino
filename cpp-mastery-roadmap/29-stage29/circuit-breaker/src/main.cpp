#include "circuit_breaker.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Circuit Breaker Demo ===\n\n";

    circuit_breaker::CircuitBreaker cb(3, std::chrono::milliseconds(100));

    std::cout << "State: " << static_cast<int>(cb.state()) << " (0=closed)\n";

    for (int i = 0; i < 3; ++i) {
        try { cb.execute([]() -> int { throw std::runtime_error("fail"); }); }
        catch (...) { std::cout << "Failure " << i + 1 << "\n"; }
    }

    std::cout << "State: " << static_cast<int>(cb.state()) << " (1=open)\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    auto result = cb.execute([]() { return 42; });
    std::cout << "After timeout, result: " << result << "\n";
    std::cout << "State: " << static_cast<int>(cb.state()) << " (0=closed)\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
