#include "thread_pool_advanced.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <atomic>
#include <numeric>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_submit_and_future() {
    thread_pool_advanced::ThreadPool pool(2);
    auto future = pool.submit([]() { return 42; });
    expect(future.get() == 42, "expected 42");
}

void test_concurrent_sum() {
    thread_pool_advanced::ThreadPool pool(4);
    std::vector<std::future<int>> futures;

    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.submit([i]() { return i * i; }));
    }

    int sum = 0;
    for (auto& f : futures) {
        sum += f.get();
    }
    expect(sum == 285, "expected sum 285 (0+1+4+9+16+25+36+49+64+81)");
}

void test_thread_count() {
    thread_pool_advanced::ThreadPool pool(4);
    expect(pool.threadCount() == 4, "expected 4 threads");
}

void test_pending_tasks() {
    thread_pool_advanced::ThreadPool pool(1);
    std::atomic<int> counter{0};

    for (int i = 0; i < 10; ++i) {
        pool.submit([&counter]() {
            counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    expect(counter == 10, "expected 10 tasks completed");
}

} // namespace

int main() {
    try {
        std::cout << "Thread Pool Advanced Tests:\n";
        test_submit_and_future();
        std::cout << "  test_submit_and_future: PASS\n";

        test_concurrent_sum();
        std::cout << "  test_concurrent_sum: PASS\n";

        test_thread_count();
        std::cout << "  test_thread_count: PASS\n";

        test_pending_tasks();
        std::cout << "  test_pending_tasks: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "thread_pool_advanced_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nthread_pool_advanced_tests passed" << std::endl;
    return 0;
}
