#include "thread_safe_cache.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <atomic>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_set_get() {
    thread_safe_cache::Cache<std::string, int> cache;

    cache.set("key1", 42);
    auto value = cache.get("key1");
    expect(value.has_value(), "expected value to exist");
    expect(value.value() == 42, "expected 42");
}

void test_get_missing() {
    thread_safe_cache::Cache<std::string, int> cache;

    auto value = cache.get("missing");
    expect(!value.has_value(), "expected missing key");
}

void test_overwrite() {
    thread_safe_cache::Cache<std::string, int> cache;

    cache.set("key", 1);
    cache.set("key", 2);

    auto value = cache.get("key");
    expect(value.value() == 2, "expected overwritten value");
}

void test_erase() {
    thread_safe_cache::Cache<std::string, int> cache;

    cache.set("key", 42);
    expect(cache.erase("key"), "expected erase to succeed");
    expect(!cache.get("key").has_value(), "expected key to be gone");
}

void test_contains() {
    thread_safe_cache::Cache<std::string, int> cache;

    cache.set("key", 42);
    expect(cache.contains("key"), "expected contains to return true");
    expect(!cache.contains("missing"), "expected contains to return false");
}

void test_size() {
    thread_safe_cache::Cache<std::string, int> cache;

    expect(cache.size() == 0, "expected size 0");
    cache.set("a", 1);
    expect(cache.size() == 1, "expected size 1");
    cache.set("b", 2);
    expect(cache.size() == 2, "expected size 2");
}

void test_max_size() {
    thread_safe_cache::Cache<std::string, int> cache(3);

    cache.set("a", 1);
    cache.set("b", 2);
    cache.set("c", 3);
    expect(cache.size() == 3, "expected size 3");

    cache.set("d", 4);  // Should evict oldest
    expect(cache.size() == 3, "expected size 3 after eviction");
    expect(!cache.contains("a"), "expected oldest to be evicted");
}

void test_clear() {
    thread_safe_cache::Cache<std::string, int> cache;

    cache.set("a", 1);
    cache.set("b", 2);

    cache.clear();
    expect(cache.size() == 0, "expected size 0 after clear");
}

void test_get_or_else() {
    thread_safe_cache::Cache<std::string, int> cache;

    expect(cache.getOrElse("missing", 42) == 42, "expected default value");

    cache.set("key", 100);
    expect(cache.getOrElse("key", 42) == 100, "expected stored value");
}

void test_get_or_create() {
    thread_safe_cache::Cache<std::string, int> cache;
    int createCount = 0;

    auto creator = [&createCount]() {
        createCount++;
        return 42;
    };

    int val1 = cache.getOrCreate("key", creator);
    expect(val1 == 42, "expected created value");
    expect(createCount == 1, "expected creator to be called once");

    int val2 = cache.getOrCreate("key", creator);
    expect(val2 == 42, "expected cached value");
    expect(createCount == 1, "expected creator not to be called again");
}

void test_concurrent_access() {
    thread_safe_cache::Cache<int, int> cache(100);
    std::atomic<int> completed{0};

    std::vector<std::thread> threads;

    // Writers
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&cache, &completed, i]() {
            for (int j = 0; j < 20; ++j) {
                cache.set(i * 20 + j, j);
            }
            completed++;
        });
    }

    // Readers
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&cache, &completed, i]() {
            for (int j = 0; j < 20; ++j) {
                cache.get(i * 20 + j);
            }
            completed++;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    expect(completed == 10, "expected 10 completed threads");
}

} // namespace

int main() {
    try {
        std::cout << "Thread-Safe Cache Tests:\n";
        test_basic_set_get();
        std::cout << "  test_basic_set_get: PASS\n";

        test_get_missing();
        std::cout << "  test_get_missing: PASS\n";

        test_overwrite();
        std::cout << "  test_overwrite: PASS\n";

        test_erase();
        std::cout << "  test_erase: PASS\n";

        test_contains();
        std::cout << "  test_contains: PASS\n";

        test_size();
        std::cout << "  test_size: PASS\n";

        test_max_size();
        std::cout << "  test_max_size: PASS\n";

        test_clear();
        std::cout << "  test_clear: PASS\n";

        test_get_or_else();
        std::cout << "  test_get_or_else: PASS\n";

        test_get_or_create();
        std::cout << "  test_get_or_create: PASS\n";

        test_concurrent_access();
        std::cout << "  test_concurrent_access: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "thread_safe_cache_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nthread_safe_cache_tests passed" << std::endl;
    return 0;
}
