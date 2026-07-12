#include "object_pool_advanced.hpp"

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

struct TestObject {
    int id;
    bool valid = true;
    TestObject() : id(0), valid(true) {}
    explicit TestObject(int i) : id(i), valid(true) {}
};

void test_basic_acquire_release() {
    int nextId = 0;
    object_pool_advanced::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        [](const TestObject* p) { return p->valid; },
        2, 5
    );

    auto obj1 = pool.acquire();
    expect(obj1->id == 0, "expected first object");

    auto obj2 = pool.acquire();
    expect(obj2->id == 1, "expected second object");
}

void test_pool_validation() {
    int nextId = 0;
    object_pool_advanced::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        [](const TestObject* p) { return p->valid; },
        2, 5
    );

    auto obj = pool.acquire();
    obj->valid = false;
    obj.reset();

    // Should create new object since old one was invalid
    auto obj2 = pool.acquire();
    expect(obj2->valid, "expected new valid object");
}

void test_pool_size() {
    int nextId = 0;
    object_pool_advanced::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        [](const TestObject* p) { return true; },
        3, 10
    );

    expect(pool.poolSize() == 3, "expected pool size 3");

    auto obj = pool.acquire();
    expect(pool.poolSize() == 2, "expected pool size 2 after acquire");

    obj.reset();
    expect(pool.poolSize() == 3, "expected pool size 3 after release");
}

void test_concurrent_access() {
    int nextId = 0;
    object_pool_advanced::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        [](const TestObject* p) { return p->valid; },
        5, 20
    );

    std::vector<std::thread> threads;
    std::atomic<int> completed{0};

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&pool, &completed]() {
            auto obj = pool.acquire();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            obj.reset();
            completed++;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    expect(completed == 10, "expected 10 completed operations");
}

} // namespace

int main() {
    try {
        std::cout << "Object Pool Advanced Tests:\n";
        test_basic_acquire_release();
        std::cout << "  test_basic_acquire_release: PASS\n";

        test_pool_validation();
        std::cout << "  test_pool_validation: PASS\n";

        test_pool_size();
        std::cout << "  test_pool_size: PASS\n";

        test_concurrent_access();
        std::cout << "  test_concurrent_access: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "object_pool_advanced_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobject_pool_advanced_tests passed" << std::endl;
    return 0;
}
