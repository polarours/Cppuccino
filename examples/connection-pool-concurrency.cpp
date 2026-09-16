// examples/connection-pool-concurrency.cpp
// Demonstrates ConnectionPool concurrent access and thread safety.
// Compile: g++ -std=c++20 -pthread -O2 -o connection-pool-concurrency connection-pool-concurrency.cpp

#include "../cpp-mastery-roadmap/06-stage6/connection-pool/include/connection_pool.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// Simulated connection
struct Connection {
    int id;
    bool operator==(const Connection& other) const { return id == other.id; }
};

Connection makeConnection() {
    static int counter = 0;
    return {++counter};
}

void destroyConnection(Connection* conn) {
    delete conn;
}

int main() {
    std::cout << "=== Connection Pool Concurrency Demo ===\n\n";

    // Create pool with min 2, max 5 connections, 1s timeout
    connection_pool::ConnectionPool<Connection> pool(
        [](){ return makeConnection(); },
        2,   // min size
        5,   // max size
        std::chrono::milliseconds(1000)
    );

    std::cout << "Initial pool size: " << pool.size() << "\n";
    std::cout << "Active connections: " << pool.activeConnections() << "\n";

    // Concurrent acquisition
    std::vector<std::thread> threads;
    std::atomic<int> acquired{0};

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            try {
                auto conn = pool.acquire();
                acquired++;
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                pool.release(conn);
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        });
    }

    for (auto& t : threads) t.join();

    std::cout << "\nAcquired connections: " << acquired << "\n";
    std::cout << "Pool size after: " << pool.size() << "\n";
    std::cout << "Active after: " << pool.activeConnections() << "\n";

    // Test timeout behavior
    std::cout << "\nTesting timeout (all connections held)...\n";
    std::vector<Connection> held;
    for (int i = 0; i < 5; ++i) {
        held.push_back(pool.acquire());
    }

    auto start = std::chrono::steady_clock::now();
    bool timedOut = false;
    try {
        auto conn = pool.acquire(); // should timeout
    } catch (const std::runtime_error& e) {
        timedOut = true;
        std::cout << "Timeout caught: " << e.what() << "\n";
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
    std::cout << "Elapsed: " << elapsed << " ms (expected ~1000 ms)\n";

    // Release and close
    for (auto& conn : held) {
        pool.release(conn);
    }
    pool.close();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
