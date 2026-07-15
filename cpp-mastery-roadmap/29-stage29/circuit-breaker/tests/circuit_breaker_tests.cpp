#include "circuit_breaker.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_closed_state() {
    circuit_breaker::CircuitBreaker cb(3, std::chrono::milliseconds(50));

    auto result = cb.execute([]() { return 42; });
    expect(result == 42, "expected 42");
    expect(cb.state() == circuit_breaker::State::Closed, "expected Closed");
}

void test_opens_after_failures() {
    circuit_breaker::CircuitBreaker cb(3, std::chrono::milliseconds(50));

    for (int i = 0; i < 3; ++i) {
        try { cb.execute([]() -> int { throw std::runtime_error("fail"); }); }
        catch (...) {}
    }

    expect(cb.state() == circuit_breaker::State::Open, "expected Open after 3 failures");
    expect(cb.failureCount() == 3, "expected 3 failures");
}

void test_open_rejects() {
    circuit_breaker::CircuitBreaker cb(2, std::chrono::milliseconds(500));

    try { cb.execute([]() -> int { throw std::runtime_error("fail"); }); }
    catch (...) {}
    try { cb.execute([]() -> int { throw std::runtime_error("fail"); }); }
    catch (...) {}

    bool threw = false;
    try {
        cb.execute([]() { return 1; });
    } catch (const std::runtime_error& e) {
        threw = true;
        expect(std::string(e.what()).find("open") != std::string::npos, "expected open message");
    }
    expect(threw, "expected rejection");
}

void test_half_open_recovery() {
    circuit_breaker::CircuitBreaker cb(2, std::chrono::milliseconds(10));

    try { cb.execute([]() -> int { throw std::runtime_error("fail"); }); }
    catch (...) {}
    try { cb.execute([]() -> int { throw std::runtime_error("fail"); }); }
    catch (...) {}

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    auto result = cb.execute([]() { return 1; });
    expect(result == 1, "expected success in half-open");
    expect(cb.state() == circuit_breaker::State::Closed, "expected Closed after recovery");
}

} // namespace

int main() {
    try {
        std::cout << "Circuit Breaker Tests:\n";
        test_closed_state();
        std::cout << "  test_closed_state: PASS\n";

        test_opens_after_failures();
        std::cout << "  test_opens_after_failures: PASS\n";

        test_open_rejects();
        std::cout << "  test_open_rejects: PASS\n";

        test_half_open_recovery();
        std::cout << "  test_half_open_recovery: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "circuit_breaker_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncircuit_breaker_tests passed" << std::endl;
    return 0;
}
