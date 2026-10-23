// examples/cpp20-coroutines-complete.cpp
// Demonstrates C++20 coroutines: generators and tasks.
// Compile with: g++ -std=c++20 -o cpp20_coroutines_complete cpp20-coroutines-complete.cpp


#include <iostream>
#include <coroutine>
#include <exception>
#include <optional>
#include <string>
#include <utility>

// A simple coroutine: a generator
template<typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator{handle_type::from_promise(*this)};
        }
        
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_value(T value) {
            current_value = value;
        }

        // Required for co_yield
        std::suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }

        void unhandled_exception() {
            std::terminate();
        }
    };
    
    using handle_type = std::coroutine_handle<promise_type>;
    
    handle_type handle_;
    
    explicit Generator(handle_type handle) : handle_(handle) {}
    
    ~Generator() {
        if (handle_) handle_.destroy();
    }
    
    // Copying is not allowed
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    
    // Moving is allowed
    Generator(Generator&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }
    
    // The iterator interface
    struct Iterator {
        handle_type handle;

        T& operator*() { return handle.promise().current_value; }
        T* operator->() { return &handle.promise().current_value; }

        // Sentinel semantics: the handle of the end() iterator is empty.
        bool operator==(const Iterator& other) const {
            if (!other.handle) return handle ? handle.done() : true;
            if (!handle) return other.handle.done();
            return handle == other.handle;
        }
        bool operator!=(const Iterator& other) const { return !(*this == other); }

        Iterator& operator++() {
            // Keeps it from being resumed after the final suspend (that would be UB)
            if (handle && !handle.done()) handle.resume();
            return *this;
        }
    };
    
    Iterator begin() {
        handle_.resume();
        return Iterator{handle_};
    }
    
    Iterator end() { return Iterator{nullptr}; }
};

// Using it: the Fibonacci sequence
Generator<int> fibonacci(int n) {
    int a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        co_yield a;
        std::swap(a, b);
        b += a;
    }
}

// A simple coroutine: an asynchronous task
class Task {
public:
    struct promise_type {
        std::exception_ptr exc_;
        int result_ = 0;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // Both suspend (a lazy Task): the frame stays alive, and the destructor
        // releases it with destroy(). If the final suspend used suspend_never, the
        // frame would be reclaimed by the runtime at the co_return, and calling
        // destroy() on it afterwards would be a double destroy -> SIGSEGV.
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        void return_value(int value) { result_ = value; }

        void unhandled_exception() {
            exc_ = std::current_exception();
        }
    };
    
    using handle_type = std::coroutine_handle<promise_type>;
    
    explicit Task(handle_type handle) : handle_(handle) {}
    
    ~Task() {
        if (handle_) handle_.destroy();
    }
    
    int get_result() {
        if (handle_.promise().exc_) {
            std::rethrow_exception(handle_.promise().exc_);
        }
        return handle_.promise().result_;
    }

    bool is_done() const { return handle_.done(); }

    void resume() { handle_.resume(); }

private:
    handle_type handle_;
};

// An asynchronous computation
Task async_compute() {
    // Stands in for an asynchronous operation
    co_return 42;
}

// Using the coroutine
void demonstrateGenerator() {
    std::cout << "=== Generator Demo ===\n";
    auto fib = fibonacci(10);
    std::cout << "Fibonacci: ";
    for (int v : fib) {
        std::cout << v << " ";
    }
    std::cout << "\n\n";
}

void demonstrateTask() {
    std::cout << "=== Task Demo ===\n";
    auto task = async_compute();
    task.resume();  // Lazy task: run it to the (final) suspension point
    std::cout << "Result: " << task.get_result() << "\n\n";
}

// A coroutine that returns a value
template<typename T>
class Promise {
public:
    using handle_type = std::coroutine_handle<Promise>;
    
    Promise() : resume_(nullptr) {}
    ~Promise() { if (resume_) resume_.destroy(); }
    
    void resume() { if (resume_) resume_.resume(); }
    void set_resume(handle_type h) { resume_ = h; }
    
    T get_value() const { return value_; }
    void set_value(T v) { value_ = std::move(v); }
    
private:
    handle_type resume_;
    T value_;
};

// A simple chain of coroutines
class Chain {
public:
    struct promise_type {
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(int) {}
        std::suspend_always yield_value(int) { return {}; }
        void unhandled_exception() { std::terminate(); }
        
        Chain get_return_object() {
            return Chain{handle_type::from_promise(*this)};
        }
    };
    
    using handle_type = std::coroutine_handle<promise_type>;
    
    explicit Chain(handle_type h) : handle_(h) {}
    ~Chain() { if (handle_) handle_.destroy(); }
    
    void start() { handle_.resume(); }
    bool is_done() const { return handle_.done(); }

private:
    handle_type handle_;
};

Chain chained_coroutine(int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << "Coroutine step: " << i << "\n";
        co_yield i;
    }
}

int main() {
    std::cout << "=== C++20 Coroutines Demo ===\n\n";
    
    demonstrateGenerator();
    demonstrateTask();
    
    std::cout << "=== Chained Coroutine Demo ===\n";
    auto chain = chained_coroutine(5);
    chain.start();
    
    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
