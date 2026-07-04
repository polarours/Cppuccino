// examples/coroutine_basics.cpp
// Demonstrates C++20 coroutine fundamentals.
// Compile with: g++ -std=c++20 -fcoroutines -o coroutine_basics coroutine_basics.cpp

#include <iostream>
#include <coroutine>
#include <optional>
#include <string>

// --- 1. Generator Coroutine ---

template <typename T>
class Generator {
public:
    struct promise_type {
        T current_value;

        Generator get_return_object() {
            return Generator{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }

        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    explicit Generator(std::coroutine_handle<promise_type> h) : handle_(h) {}

    ~Generator() {
        if (handle_) handle_.destroy();
    }

    // Non-copyable, movable
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

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

    const T& current() const {
        return handle_.promise().current_value;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

// Fibonacci generator
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
    std::cout << "--- Generator Coroutine ---\n";

    auto gen = fibonacci(10);
    while (gen.next()) {
        std::cout << gen.current() << " ";
    }
    std::cout << "\n";
}

// --- 2. Lazy Evaluation ---

Generator<int> range(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}

void lazyEvalDemo() {
    std::cout << "\n--- Lazy Evaluation ---\n";

    // Numbers are generated on-demand
    auto gen = range(0, 5);
    while (gen.next()) {
        std::cout << gen.current() << " ";
    }
    std::cout << "\n";
}

// --- 3. Task Coroutine (Simplified) ---

class Task {
public:
    struct promise_type {
        int result;

        Task get_return_object() {
            return Task{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_value(int value) {
            result = value;
        }

        void unhandled_exception() { std::terminate(); }
    };

    explicit Task(std::coroutine_handle<promise_type> h) : handle_(h) {}

    ~Task() {
        if (handle_) handle_.destroy();
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    int result() {
        return handle_.promise().result;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

Task computeValue(int x) {
    co_return x * 2;
}

void taskDemo() {
    std::cout << "\n--- Task Coroutine ---\n";

    auto task = computeValue(21);
    std::cout << "computeValue(21) = " << task.result() << "\n";
}

// --- 4. Coroutine with String Result ---

Generator<std::string> greetNames() {
    co_yield "Hello, ";
    co_yield "World";
    co_yield "!";
}

void stringGeneratorDemo() {
    std::cout << "\n--- String Generator ---\n";

    auto gen = greetNames();
    std::string result;
    while (gen.next()) {
        result += gen.current();
    }
    std::cout << result << "\n";
}

// --- 5. Coroutine Lifecycle ---

void lifecycleDemo() {
    std::cout << "\n--- Coroutine Lifecycle ---\n";
    std::cout << "1. Coroutine created (promise allocated)\n";
    std::cout << "2. initial_suspend: suspend_always = lazy start\n";
    std::cout << "3. resume() called → body executes\n";
    std::cout << "4. co_yield: suspends, returns value\n";
    std::cout << "5. resume() called → continues\n";
    std::cout << "6. co_return: suspends at final_suspend\n";
    std::cout << "7. Coroutine destroyed (promise freed)\n";
}

// --- 6. Infinite Generator ---

Generator<int> infiniteCounter(int start) {
    int i = start;
    while (true) {
        co_yield i++;
    }
}

void infiniteDemo() {
    std::cout << "\n--- Infinite Generator (limited) ---\n";

    auto gen = infiniteCounter(100);
    for (int i = 0; i < 5; ++i) {
        gen.next();
        std::cout << gen.current() << " ";
    }
    std::cout << "\n";
}

int main() {
    generatorDemo();
    lazyEvalDemo();
    taskDemo();
    stringGeneratorDemo();
    lifecycleDemo();
    infiniteDemo();

    std::cout << "\n=== All coroutine scenarios completed ===\n";
    return 0;
}
