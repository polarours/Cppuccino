#include "read_write_lock.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_insert_get() {
    read_write_lock::ThreadSafeMap<std::string, int> map;
    map.insert("a", 1);
    map.insert("b", 2);

    expect(map.get("a") == 1, "expected 1");
    expect(map.get("b") == 2, "expected 2");
    expect(map.size() == 2, "expected 2");
}

void test_concurrent_read_write() {
    read_write_lock::ThreadSafeMap<int, int> map;

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&map, i]() {
            map.insert(i, i * 10);
        });
    }
    for (auto& t : threads) t.join();

    expect(map.size() == 10, "expected 10 entries");
}

void test_erase() {
    read_write_lock::ThreadSafeMap<std::string, int> map;
    map.insert("x", 100);
    expect(map.erase("x"), "expected erase success");
    expect(!map.contains("x"), "expected not found after erase");
    expect(map.size() == 0, "expected 0 size");
}

void test_keys() {
    read_write_lock::ThreadSafeMap<std::string, int> map;
    map.insert("a", 1);
    map.insert("b", 2);
    map.insert("c", 3);

    auto k = map.keys();
    expect(k.size() == 3, "expected 3 keys");
}

} // namespace

int main() {
    try {
        std::cout << "Read Write Lock Tests:\n";
        test_insert_get();
        std::cout << "  test_insert_get: PASS\n";

        test_concurrent_read_write();
        std::cout << "  test_concurrent_read_write: PASS\n";

        test_erase();
        std::cout << "  test_erase: PASS\n";

        test_keys();
        std::cout << "  test_keys: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "read_write_lock_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nread_write_lock_tests passed" << std::endl;
    return 0;
}
