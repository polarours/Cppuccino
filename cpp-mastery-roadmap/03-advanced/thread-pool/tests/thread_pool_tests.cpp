#include "thread_pool.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <atomic>
#include <chrono>
#include <vector>
#include <numeric>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_enqueue() {
    thread_pool::ThreadPool pool(2);

    auto future = pool.enqueue([]() {
        return 42;
    });

    expect(future.get() == 42, "expected result 42");
}

void test_multiple_tasks() {
    thread_pool::ThreadPool pool(4);

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.enqueue([i]() {
            return i * i;
        }));
    }

    for (int i = 0; i < 10; ++i) {
        expect(futures[i].get() == i * i, "expected " + std::to_string(i * i));
    }
}

void test_void_tasks() {
    thread_pool::ThreadPool pool(2);
    std::atomic<int> counter{0};

    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.enqueue([&counter]() {
            counter++;
        }));
    }

    for (auto& f : futures) {
        f.get();
    }

    expect(counter == 10, "expected counter to be 10");
}

void test_task_with_args() {
    thread_pool::ThreadPool pool(2);

    auto future = pool.enqueue([](int a, int b) {
        return a + b;
    }, 3, 4);

    expect(future.get() == 7, "expected 7");
}

void test_wait_for_all() {
    thread_pool::ThreadPool pool(2);
    std::atomic<int> counter{0};

    for (int i = 0; i < 20; ++i) {
        pool.enqueue([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            counter++;
        });
    }

    pool.waitForAll();
    expect(counter == 20, "expected all tasks completed");
}

void test_thread_count() {
    thread_pool::ThreadPool pool(4);
    expect(pool.threadCount() == 4, "expected 4 threads");
}

void test_shutdown() {
    thread_pool::ThreadPool pool(2);
    std::atomic<int> counter{0};

    for (int i = 0; i < 10; ++i) {
        pool.enqueue([&counter]() {
            counter++;
        });
    }

    pool.shutdown();
    expect(pool.isShutdown(), "expected shutdown");
}

void test_exception_in_task() {
    thread_pool::ThreadPool pool(2);

    auto future = pool.enqueue([]() -> int {
        throw std::runtime_error("task error");
        return 0;
    });

    try {
        future.get();
        expect(false, "expected exception");
    } catch (const std::runtime_error& e) {
        expect(std::string(e.what()) == "task error", "expected correct error message");
    }
}

void test_concurrent_increment() {
    thread_pool::ThreadPool pool(4);
    std::atomic<int> counter{0};

    std::vector<std::future<void>> futures;
    for (int i = 0; i < 1000; ++i) {
        futures.push_back(pool.enqueue([&counter]() {
            counter++;
        }));
    }

    for (auto& f : futures) {
        f.get();
    }

    expect(counter == 1000, "expected 1000 increments");
}

void test_performance() {
    thread_pool::ThreadPool pool(4);

    auto start = std::chrono::steady_clock::now();

    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10000; ++i) {
        futures.push_back(pool.enqueue([]() {
            return 1;
        }));
    }

    int sum = 0;
    for (auto& f : futures) {
        sum += f.get();
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    expect(sum == 10000, "expected sum 10000");
    std::cout << "  10000 tasks completed in " << duration.count() << "ms\n";
}

} // namespace

int main() {
    try {
        std::cout << "ThreadPool Tests:\n";
        test_basic_enqueue();
        std::cout << "  test_basic_enqueue: PASS\n";

        test_multiple_tasks();
        std::cout << "  test_multiple_tasks: PASS\n";

        test_void_tasks();
        std::cout << "  test_void_tasks: PASS\n";

        test_task_with_args();
        std::cout << "  test_task_with_args: PASS\n";

        test_wait_for_all();
        std::cout << "  test_wait_for_all: PASS\n";

        test_thread_count();
        std::cout << "  test_thread_count: PASS\n";

        test_shutdown();
        std::cout << "  test_shutdown: PASS\n";

        test_exception_in_task();
        std::cout << "  test_exception_in_task: PASS\n";

        test_concurrent_increment();
        std::cout << "  test_concurrent_increment: PASS\n";

        test_performance();
        std::cout << "  test_performance: PASS\n";
    } catch (const std::exception& exception) {
        std::cerr << "thread_pool_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nthread_pool_tests passed" << std::endl;
    return 0;
}
