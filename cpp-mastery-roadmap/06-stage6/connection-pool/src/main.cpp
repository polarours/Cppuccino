#include "connection_pool.hpp"

#include <iostream>
#include <thread>
#include <chrono>

struct DatabaseConnection {
    int id;
    DatabaseConnection() : id(0) {}
    explicit DatabaseConnection(int i) : id(i) {}
    void query(const std::string& q) {
        std::cout << "  Connection " << id << " executing: " << q << "\n";
    }
};

int main() {
    std::cout << "=== Connection Pool Demo ===\n\n";

    int nextId = 0;
    connection_pool::ConnectionPool<DatabaseConnection> pool(
        [&nextId]() { return DatabaseConnection(nextId++); },
        2, 5
    );

    std::cout << "Pool size: " << pool.size() << "\n\n";

    // Simulate concurrent access
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&pool, i]() {
            auto conn = pool.acquire();
            conn.query("SELECT * FROM users");
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            pool.release(conn);
            std::cout << "Thread " << i << " released connection\n";
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\nPool size after all released: " << pool.size() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
