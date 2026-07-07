// examples/cpp20-coroutines.cpp
// Demonstrates C++20 coroutine basics.
// Compile with: g++ -std=c++20 -fcoroutines -o cpp20_coroutines cpp20-coroutines.cpp

#include <iostream>
#include <coroutine>
#include <optional>
#include <string>
#include <vector>

// --- 1. Generator ---

template <typename T>
class Generator {
public:
    struct promise_type {
        T value;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T v) {
            value = v;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
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

    const T& current() const { return handle_.promise().value; }

private:
    std::coroutine_handle<promise_type> handle_;
};

Generator<int> fibonacci(int count) {
    int a = 0, b = 1;
    for (int i = 0; i < count; ++i) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

void generatorDemo() {
    std::cout << "--- Generator ---\n";
    auto gen = fibonacci(10);
    while (gen.next()) {
        std::cout << gen.current() << " ";
    }
    std::cout << "\n";
}

// --- 2. Range Generator ---

Generator<int> range(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}

void rangeDemo() {
    std::cout << "\n--- Range Generator ---\n";
    auto gen = range(0, 5);
    while (gen.next()) {
        std::cout << gen.current() << " ";
    }
    std::cout << "\n";
}

// --- 3. Task ---

class Task {
public:
    struct promise_type {
        int result;
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(int v) { result = v; }
        void unhandled_exception() { std::terminate(); }
    };

    explicit Task(std::coroutine_handle<promise_type> h) : handle_(h) {}
    ~Task() { if (handle_) handle_.destroy(); }
    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }
    int result() { return handle_.promise().result; }

private:
    std::coroutine_handle<promise_type> handle_;
};

Task compute(int x) {
    co_return x * 2;
}

void taskDemo() {
    std::cout << "\n--- Task ---\n";
    auto task = compute(21);
    std::cout << "compute(21) = " << task.result() << "\n";
}

// --- 4. Infinite Generator ---

Generator<int> counter(int start) {
    int i = start;
    while (true) {
        co_yield i++;
    }
}

void infiniteDemo() {
    std::cout << "\n--- Infinite Generator (limited) ---\n";
    auto gen = counter(100);
    for (int i = 0; i < 5; ++i) {
        gen.next();
        std::cout << gen.current() << " ";
    }
    std::cout << "\n";
}

// --- 5. String Generator ---

Generator<std::string> greetNames() {
    co_yield "Hello, ";
    co_yield "World";
    co_yield "!";
}

void stringDemo() {
    std::cout << "\n--- String Generator ---\n";
    auto gen = greetNames();
    std::string result;
    while (gen.next()) {
        result += gen.current();
    }
    std::cout << result << "\n";
}

// --- 6. Coroutine Lifecycle ---

void lifecycleDemo() {
    std::cout << "\n--- Coroutine Lifecycle ---\n";
    std::cout << "1. Created (promise allocated)\n";
    std::cout << "2. initial_suspend: suspend_always = lazy start\n";
    std::cout << "3. resume() → body executes\n";
    std::cout << "4. co_yield: suspends, returns value\n";
    std::cout << "5. resume() → continues\n";
    std::cout << "6. co_return: suspends at final_suspend\n";
    std::cout << "7. Destroyed (promise freed)\n";
}

int main() {
    generatorDemo();
    rangeDemo();
    taskDemo();
    infiniteDemo();
    stringDemo();
    lifecycleDemo();

    std::cout << "\n=== All coroutine scenarios completed ===\n";
    return 0;
}
