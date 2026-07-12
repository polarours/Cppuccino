#include "object_pool_advanced.hpp"

#include <iostream>
#include <string>
#include <memory>

struct Connection {
    int id;
    bool valid = true;
    Connection() : id(0), valid(true) {}
    explicit Connection(int i) : id(i), valid(true) {}
};

int main() {
    std::cout << "=== Object Pool Advanced Demo ===\n\n";

    int nextId = 0;
    object_pool_advanced::ObjectPool<Connection> pool(
        [&nextId]() { return new Connection(nextId++); },
        [](Connection* p) { delete p; },
        [](const Connection* p) { return p->valid; },
        2, 5
    );

    std::cout << "Pool size: " << pool.poolSize() << "\n\n";

    auto conn1 = pool.acquire();
    auto conn2 = pool.acquire();
    std::cout << "Acquired 2 connections\n";
    std::cout << "Pool size: " << pool.poolSize() << "\n";

    conn1.reset();
    conn2.reset();
    std::cout << "Released connections\n";
    std::cout << "Pool size: " << pool.poolSize() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
