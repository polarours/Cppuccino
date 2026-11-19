#include "strategy_cache.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_lru_cache() {
    strategy_cache::Cache<std::string, int> cache;
    cache.setStrategy(std::make_shared<strategy_cache::LRUStrategy<std::string, int>>(3));

    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("c", 3);

    auto val = cache.get("a");
    expect(val.has_value(), "expected value");
    expect(val.value() == 1, "expected 1");

    // Add one more, should evict oldest
    cache.put("d", 4);
    expect(cache.size() == 3, "expected size 3");
}

// Regression: eviction removed map.begin(), the smallest key, not the least
// recently used one, and a read did not count as a use.
void test_lru_evicts_by_recency() {
    strategy_cache::LRUStrategy<std::string, int> lru(3);

    lru.put("a", 1);
    lru.put("b", 2);
    lru.put("c", 3);
    lru.get("a");  // 'a' becomes the most recently used
    lru.put("d", 4);

    expect(lru.get("a").has_value(), "expected 'a' to survive the read");
    expect(!lru.get("b").has_value(), "expected 'b' to be evicted, not 'a'");
    expect(lru.get("c").has_value(), "expected 'c' to survive");
    expect(lru.get("d").has_value(), "expected 'd' to be stored");
}

// A put over an existing key refreshes it too. Separate instance, so the
// get() calls in the assertions above cannot shape this eviction.
void test_lru_put_refreshes() {
    strategy_cache::LRUStrategy<std::string, int> lru(3);

    lru.put("x", 1);
    lru.put("y", 2);
    lru.put("z", 3);
    lru.put("x", 10);  // refresh 'x', so 'y' is the least recently used
    lru.put("w", 4);

    expect(lru.get("x").has_value(), "expected refreshed 'x' to survive");
    expect(!lru.get("y").has_value(), "expected 'y' to be evicted");
    expect(lru.get("z").has_value(), "expected 'z' to survive");
    expect(lru.get("w").has_value(), "expected 'w' to be stored");
}

// Regression: a zero-capacity strategy used to call map::erase(end()).
void test_lru_zero_capacity() {
    strategy_cache::LRUStrategy<std::string, int> lru(0);
    lru.put("x", 1);
    expect(lru.size() == 0, "expected a zero-capacity strategy to store nothing");
    expect(!lru.get("x").has_value(), "expected nothing to come back");
}

void test_ttl_cache() {
    strategy_cache::Cache<std::string, int> cache;
    cache.setStrategy(std::make_shared<strategy_cache::TTLStrategy<std::string, int>>(
        std::chrono::seconds(1)));

    cache.put("key", 42);

    auto val = cache.get("key");
    expect(val.has_value(), "expected value");
    expect(val.value() == 42, "expected 42");

    // Wait for expiration
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    auto expired = cache.get("key");
    expect(!expired.has_value(), "expected expired");
}

void test_strategy_switch() {
    strategy_cache::Cache<std::string, int> cache;

    // Start with LRU
    cache.setStrategy(std::make_shared<strategy_cache::LRUStrategy<std::string, int>>(5));
    cache.put("key", 42);
    expect(cache.get("key").value() == 42, "expected 42");

    // Switch to TTL
    cache.setStrategy(std::make_shared<strategy_cache::TTLStrategy<std::string, int>>(
        std::chrono::seconds(1)));
    cache.put("key2", 100);
    expect(cache.get("key2").value() == 100, "expected 100");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Cache Tests:\n";
        test_lru_cache();
        std::cout << "  test_lru_cache: PASS\n";

        test_lru_evicts_by_recency();
        std::cout << "  test_lru_evicts_by_recency: PASS\n";

        test_lru_put_refreshes();
        std::cout << "  test_lru_put_refreshes: PASS\n";

        test_lru_zero_capacity();
        std::cout << "  test_lru_zero_capacity: PASS\n";

        test_ttl_cache();
        std::cout << "  test_ttl_cache: PASS\n";

        test_strategy_switch();
        std::cout << "  test_strategy_switch: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "strategy_cache_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_cache_tests passed" << std::endl;
    return 0;
}
