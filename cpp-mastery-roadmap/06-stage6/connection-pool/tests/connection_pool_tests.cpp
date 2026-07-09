#include "connection_pool.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <thread>
#include <vector>
#include <atomic>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

struct MockConnection {
    int id;
    MockConnection() : id(0) {}
    explicit MockConnection(int i) : id(i) {}
};

void test_basic_acquire_release() {
    int nextId = 0;
    connection_pool::ConnectionPool<MockConnection> pool(
        [&nextId]() { return MockConnection(nextId++); },
        2, 4
    );

    auto conn1 = pool.acquire();
    expect(conn1.id == 0, "expected first connection");

    auto conn2 = pool.acquire();
    expect(conn2.id == 1, "expected second connection");

    pool.release(conn1);
    pool.release(conn2);
}

void test_pool_reuse() {
    int nextId = 0;
    connection_pool::ConnectionPool<MockConnection> pool(
        [&nextId]() { return MockConnection(nextId++); },
        2, 4
    );

    auto conn1 = pool.acquire();
    pool.release(conn1);

    auto conn2 = pool.acquire();
    // Pool should reuse connections
    expect(pool.size() < 2, "expected pool size to decrease after acquire");
}

void test_pool_size() {
    int nextId = 0;
    connection_pool::ConnectionPool<MockConnection> pool(
        [&nextId]() { return MockConnection(nextId++); },
        3, 5
    );

    expect(pool.size() == 3, "expected initial size 3");

    auto conn = pool.acquire();
    expect(pool.size() == 2, "expected size 2 after acquire");

    pool.release(conn);
    expect(pool.size() == 3, "expected size 3 after release");
}

void test_pool_max_size() {
    int nextId = 0;
    connection_pool::ConnectionPool<MockConnection> pool(
        [&nextId]() { return MockConnection(nextId++); },
        1, 2
    );

    auto conn1 = pool.acquire();
    auto conn2 = pool.acquire();

    // Pool should have created connections up to max size
    expect(pool.size() == 0, "expected pool size 0 when all connections acquired");
}

void test_concurrent_access() {
    int nextId = 0;
    connection_pool::ConnectionPool<MockConnection> pool(
        [&nextId]() { return MockConnection(nextId++); },
        2, 10
    );

    std::vector<std::thread> threads;
    std::atomic<int> completed{0};

    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&pool, &completed]() {
            auto conn = pool.acquire();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            pool.release(conn);
            completed++;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    expect(completed == 5, "expected 5 completed operations");
}

void test_pool_close() {
    int nextId = 0;
    connection_pool::ConnectionPool<MockConnection> pool(
        [&nextId]() { return MockConnection(nextId++); },
        2, 4
    );

    pool.close();

    bool threw = false;
    try {
        pool.acquire();
    } catch (const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "expected exception after close");
}

void test_pool_release_after_close() {
    int nextId = 0;
    connection_pool::ConnectionPool<MockConnection> pool(
        [&nextId]() { return MockConnection(nextId++); },
        2, 4
    );

    auto conn = pool.acquire();
    pool.close();

    // Should not throw
    pool.release(conn);
}

} // namespace

int main() {
    try {
        std::cout << "Connection Pool Tests:\n";
        test_basic_acquire_release();
        std::cout << "  test_basic_acquire_release: PASS\n";

        test_pool_reuse();
        std::cout << "  test_pool_reuse: PASS\n";

        test_pool_size();
        std::cout << "  test_pool_size: PASS\n";

        test_pool_max_size();
        std::cout << "  test_pool_max_size: PASS\n";

        test_concurrent_access();
        std::cout << "  test_concurrent_access: PASS\n";

        test_pool_close();
        std::cout << "  test_pool_close: PASS\n";

        test_pool_release_after_close();
        std::cout << "  test_pool_release_after_close: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "connection_pool_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nconnection_pool_tests passed" << std::endl;
    return 0;
}
