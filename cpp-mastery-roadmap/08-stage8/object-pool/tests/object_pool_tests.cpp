#include "object_pool.hpp"

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
    TestObject() : id(0) {}
    explicit TestObject(int i) : id(i) {}
};

void test_basic_acquire_release() {
    int nextId = 0;
    object_pool::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        2
    );

    auto obj1 = pool.acquire();
    expect(obj1->id == 0, "expected first object");

    auto obj2 = pool.acquire();
    expect(obj2->id == 1, "expected second object");
}

void test_pool_reuse() {
    int nextId = 0;
    object_pool::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        2
    );

    auto obj1 = pool.acquire();
    obj1.reset();  // Release back to pool

    auto obj2 = pool.acquire();
    // Object should be reused (pool returns existing objects)
    expect(pool.size() < 2, "expected pool size to decrease after acquire");
}

void test_pool_size() {
    int nextId = 0;
    object_pool::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        3
    );

    expect(pool.size() == 3, "expected initial size 3");

    auto obj = pool.acquire();
    expect(pool.size() == 2, "expected size 2 after acquire");

    obj.reset();
    expect(pool.size() == 3, "expected size 3 after release");
}

void test_concurrent_access() {
    int nextId = 0;
    object_pool::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        5
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

void test_shared_ptr_ownership() {
    int nextId = 0;
    object_pool::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        2
    );

    std::shared_ptr<TestObject> obj1 = pool.acquire();
    std::shared_ptr<TestObject> obj2 = obj1;  // Shared ownership

    expect(obj1.use_count() == 2, "expected use count 2");
    obj1.reset();
    expect(obj2.use_count() == 1, "expected use count 1");
    obj2.reset();  // Returns to pool when last shared_ptr is destroyed
}

void test_pool_clear() {
    int nextId = 0;
    object_pool::ObjectPool<TestObject> pool(
        [&nextId]() { return new TestObject(nextId++); },
        [](TestObject* p) { delete p; },
        3
    );

    auto obj1 = pool.acquire();
    auto obj2 = pool.acquire();
    expect(pool.size() == 1, "expected size 1 after acquiring 2");

    pool.clear();
    expect(pool.size() == 0, "expected size 0 after clear");
}

} // namespace

int main() {
    try {
        std::cout << "Object Pool Tests:\n";
        test_basic_acquire_release();
        std::cout << "  test_basic_acquire_release: PASS\n";

        test_pool_reuse();
        std::cout << "  test_pool_reuse: PASS\n";

        test_pool_size();
        std::cout << "  test_pool_size: PASS\n";

        test_concurrent_access();
        std::cout << "  test_concurrent_access: PASS\n";

        test_shared_ptr_ownership();
        std::cout << "  test_shared_ptr_ownership: PASS\n";

        test_pool_clear();
        std::cout << "  test_pool_clear: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "object_pool_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nobject_pool_tests passed" << std::endl;
    return 0;
}
