#include "lru_cache.hpp"

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

void test_basic_put_get() {
    lru_cache::LRUCache<std::string, int> cache(3);

    cache.put("key1", 42);
    auto value = cache.get("key1");
    expect(value.has_value(), "expected value to exist");
    expect(value.value() == 42, "expected 42");
}

void test_get_missing() {
    lru_cache::LRUCache<std::string, int> cache(3);

    auto value = cache.get("missing");
    expect(!value.has_value(), "expected missing key");
}

void test_update() {
    lru_cache::LRUCache<std::string, int> cache(3);

    cache.put("key", 1);
    cache.put("key", 2);

    auto value = cache.get("key");
    expect(value.value() == 2, "expected updated value");
    expect(cache.size() == 1, "expected size 1");
}

void test_erase() {
    lru_cache::LRUCache<std::string, int> cache(3);

    cache.put("key", 42);
    expect(cache.erase("key"), "expected erase to succeed");
    expect(!cache.get("key").has_value(), "expected key to be gone");
}

void test_contains() {
    lru_cache::LRUCache<std::string, int> cache(3);

    cache.put("key", 42);
    expect(cache.contains("key"), "expected contains to return true");
    expect(!cache.contains("missing"), "expected contains to return false");
}

void test_size() {
    lru_cache::LRUCache<std::string, int> cache(3);

    expect(cache.size() == 0, "expected size 0");
    cache.put("a", 1);
    expect(cache.size() == 1, "expected size 1");
    cache.put("b", 2);
    expect(cache.size() == 2, "expected size 2");
}

void test_lru_eviction() {
    lru_cache::LRUCache<std::string, int> cache(3);

    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("c", 3);
    expect(cache.size() == 3, "expected size 3");

    // Add new entry, should evict "a" (least recently used)
    cache.put("d", 4);
    expect(cache.size() == 3, "expected size 3 after eviction");
    expect(!cache.contains("a"), "expected 'a' to be evicted");
    expect(cache.contains("b"), "expected 'b' to exist");
    expect(cache.contains("c"), "expected 'c' to exist");
    expect(cache.contains("d"), "expected 'd' to exist");
}

void test_lru_order() {
    lru_cache::LRUCache<std::string, int> cache(3);

    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("c", 3);

    // Access "a" to make it most recently used
    cache.get("a");

    // Add new entry, should evict "b" (now least recently used)
    cache.put("d", 4);
    expect(!cache.contains("b"), "expected 'b' to be evicted");
    expect(cache.contains("a"), "expected 'a' to exist");
}

void test_clear() {
    lru_cache::LRUCache<std::string, int> cache(3);

    cache.put("a", 1);
    cache.put("b", 2);

    cache.clear();
    expect(cache.size() == 0, "expected size 0 after clear");
}

void test_get_or_else() {
    lru_cache::LRUCache<std::string, int> cache(3);

    expect(cache.getOrElse("missing", 42) == 42, "expected default value");

    cache.put("key", 100);
    expect(cache.getOrElse("key", 42) == 100, "expected stored value");
}

void test_concurrent_access() {
    lru_cache::LRUCache<int, int> cache(100);
    std::atomic<int> completed{0};

    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&cache, &completed, i]() {
            for (int j = 0; j < 20; ++j) {
                cache.put(i * 20 + j, j);
            }
            completed++;
        });
    }

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
        std::cout << "LRU Cache Tests:\n";
        test_basic_put_get();
        std::cout << "  test_basic_put_get: PASS\n";

        test_get_missing();
        std::cout << "  test_get_missing: PASS\n";

        test_update();
        std::cout << "  test_update: PASS\n";

        test_erase();
        std::cout << "  test_erase: PASS\n";

        test_contains();
        std::cout << "  test_contains: PASS\n";

        test_size();
        std::cout << "  test_size: PASS\n";

        test_lru_eviction();
        std::cout << "  test_lru_eviction: PASS\n";

        test_lru_order();
        std::cout << "  test_lru_order: PASS\n";

        test_clear();
        std::cout << "  test_clear: PASS\n";

        test_get_or_else();
        std::cout << "  test_get_or_else: PASS\n";

        test_concurrent_access();
        std::cout << "  test_concurrent_access: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "lru_cache_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nlru_cache_tests passed" << std::endl;
    return 0;
}
