#include "lfu_cache.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_basic_put_get() {
    lfu_cache::LFUCache<int, int> c(3);
    c.put(1, 10);
    c.put(2, 20);
    expect(c.get(1).value_or(-1) == 10, "get 1 -> 10");
    expect(c.get(2).value_or(-1) == 20, "get 2 -> 20");
    expect(!c.get(3).has_value(), "missing key");
    expect(c.size() == 2, "size 2");
}

void test_evicts_least_frequent() {
    lfu_cache::LFUCache<std::string, int> c(2);
    c.put("a", 1);
    c.put("b", 2);
    c.get("a");   // freq(a)=2, freq(b)=1
    c.put("c", 3);  // must evict b
    expect(c.get("b") == std::nullopt, "b evicted");
    expect(c.get("a").has_value(), "a kept");
    expect(c.get("c").has_value(), "c kept");
}

void test_tie_break_lru() {
    lfu_cache::LFUCache<int, int> c(2);
    c.put(1, 1);
    c.put(2, 2);       // both freq 1; 1 is LRU
    c.put(3, 3);       // evicts 1
    expect(!c.get(1).has_value(), "1 evicted (LRU among freq-1)");
    expect(c.get(2).has_value(), "2 kept");
    expect(c.get(3).has_value(), "3 kept");
}

void test_update_refreshes_frequency() {
    lfu_cache::LFUCache<int, int> c(2);
    c.put(1, 10);
    c.put(2, 20);
    c.put(1, 11);  // update: freq(1) bumps
    expect(c.get(1).value_or(-1) == 11, "value updated");
    c.put(3, 30);  // evict 2 (freq 1) not 1 (freq 2)
    expect(c.get(2) == std::nullopt, "2 evicted");
    expect(c.get(1).value_or(-1) == 11, "1 survives");
}

void test_zero_capacity() {
    lfu_cache::LFUCache<int, int> c(0);
    c.put(1, 1);
    expect(c.size() == 0, "nothing stored at capacity 0");
    expect(!c.get(1).has_value(), "no hit");
}

void test_fill_exactly_capacity_no_eviction() {
    lfu_cache::LFUCache<int, int> c(3);
    for (int i = 0; i < 3; ++i) c.put(i, i * 10);
    expect(c.size() == 3, "full");
    for (int i = 0; i < 3; ++i)
        expect(c.get(i).value_or(-1) == i * 10, "all present");
}

}  // namespace

int main() {
    try {
        test_basic_put_get();
        test_evicts_least_frequent();
        test_tie_break_lru();
        test_update_refreshes_frequency();
        test_zero_capacity();
        test_fill_exactly_capacity_no_eviction();
        std::cout << "All lfu_cache tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
