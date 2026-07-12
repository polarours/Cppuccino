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

        test_ttl_cache();
        std::cout << "  test_ttl_cache: PASS\n";

        test_strategy_switch();
        std::cout << "  test_strategy_switch: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_cache_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_cache_tests passed" << std::endl;
    return 0;
}
