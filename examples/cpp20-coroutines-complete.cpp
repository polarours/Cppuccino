// C++20 协程完整示例
// 展示协程的基本用法和特性

#include <iostream>
#include <coroutine>
#include <exception>
#include <optional>
#include <string>

// 简单协程：生成器
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
    
    // 禁止拷贝
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    
    // 允许移动
    Generator(Generator&& other) noexcept : handle_(std::exchange(other.handle_, nullptr)) {}
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = std::exchange(other.handle_, nullptr);
        }
        return *this;
    }
    
    // 迭代器接口
    struct Iterator {
        handle_type handle;
        
        T& operator*() { return handle.promise().current_value; }
        T* operator->() { return &operator*(); }
        
        bool operator==(const Iterator& other) const { return handle == other.handle; }
        bool operator!=(const Iterator& other) const { return handle != other.handle; }
        
        Iterator& operator++() {
            handle.resume();
            return *this;
        }
    };
    
    Iterator begin() {
        handle_.resume();
        return Iterator{handle_};
    }
    
    Iterator end() { return Iterator{nullptr}; }
};

// 使用示例：生成斐波那契数列
Generator<int> fibonacci(int n) {
    int a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        co_yield a;
        std::swap(a, b);
        b += a;
    }
}

// 简单协程：异步任务
class Task {
public:
    struct promise_type {
        std::exception_ptr exc_;
        
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        
        void return_value(int value) {}
        
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
        return 0;
    }
    
    bool is_done() const { return handle_.done(); }

private:
    handle_type handle_;
};

// 异步计算
Task async_compute() {
    // 模拟异步操作
    co_return 42;
}

// 协程使用示例
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
    std::cout << "Result: " << task.get_result() << "\n\n";
}

// 带返回值的协程
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

// 简单的协程链
class Chain {
public:
    struct promise_type {
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(int) {}
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
