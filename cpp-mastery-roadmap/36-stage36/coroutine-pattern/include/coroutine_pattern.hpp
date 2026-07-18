#pragma once

#include <coroutine>
#include <optional>
#include <iostream>

namespace coroutine_pattern {

template <typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        void return_void() {}
        void unhandled_exception() {}
    };

    explicit Generator(std::coroutine_handle<promise_type> h) : handle_(h) {}
    ~Generator() { if (handle_) handle_.destroy(); }

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }

    bool next() {
        if (!handle_ || handle_.done()) return false;
        handle_.resume();
        return !handle_.done();
    }

    T value() const { return handle_.promise().current_value; }

private:
    std::coroutine_handle<promise_type> handle_;
};

} // namespace coroutine_pattern
