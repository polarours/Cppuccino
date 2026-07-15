#pragma once

#include <string>
#include <functional>
#include <chrono>
#include <iostream>
#include <stdexcept>

namespace circuit_breaker {

enum class State { Closed, Open, HalfOpen };

class CircuitBreaker {
public:
    CircuitBreaker(int failureThreshold = 3, std::chrono::milliseconds resetTimeout = std::chrono::milliseconds(100))
        : failureThreshold_(failureThreshold), resetTimeout_(resetTimeout) {}

    template <typename Func>
    auto execute(Func func) -> decltype(func()) {
        if (state_ == State::Open) {
            if (std::chrono::steady_clock::now() - lastFailureTime_ >= resetTimeout_) {
                state_ = State::HalfOpen;
            } else {
                throw std::runtime_error("Circuit is open");
            }
        }

        try {
            auto result = func();
            onSuccess();
            return result;
        } catch (...) {
            onFailure();
            throw;
        }
    }

    State state() const { return state_; }
    int failureCount() const { return failureCount_; }
    int successCount() const { return successCount_; }
    void reset() { state_ = State::Closed; failureCount_ = 0; successCount_ = 0; }

private:
    void onSuccess() {
        successCount_++;
        if (state_ == State::HalfOpen) {
            state_ = State::Closed;
            failureCount_ = 0;
        }
    }

    void onFailure() {
        failureCount_++;
        lastFailureTime_ = std::chrono::steady_clock::now();
        if (failureCount_ >= failureThreshold_) {
            state_ = State::Open;
        }
    }

    int failureThreshold_;
    std::chrono::milliseconds resetTimeout_;
    State state_ = State::Closed;
    int failureCount_ = 0;
    int successCount_ = 0;
    std::chrono::steady_clock::time_point lastFailureTime_;
};

} // namespace circuit_breaker
