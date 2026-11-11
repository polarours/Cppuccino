#pragma once

#include <coroutine>
#include <stdexcept>
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

    // A move constructor on its own takes the move assignment away: the
    // copy assignment is deleted, so the generator could not be given to
    // another variable at all. The coroutine that is being overwritten
    // has to be destroyed, or it is never destroyed at all.
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    bool next() {
        if (!handle_ || handle_.done()) return false;
        handle_.resume();
        return !handle_.done();
    }

    T value() const {
        if (!handle_) {
            throw std::logic_error("the generator was moved from");
        }
        return handle_.promise().current_value;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

} // namespace coroutine_pattern
