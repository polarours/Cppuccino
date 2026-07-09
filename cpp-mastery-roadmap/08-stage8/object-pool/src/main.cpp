#include "object_pool.hpp"

#include <iostream>
#include <string>
#include <thread>

struct Connection {
    int id;
    Connection() : id(0) {}
    explicit Connection(int i) : id(i) {}
    void query(const std::string& q) {
        std::cout << "  Connection " << id << ": " << q << "\n";
    }
};

int main() {
    std::cout << "=== Object Pool Demo ===\n\n";

    int nextId = 0;
    object_pool::ObjectPool<Connection> pool(
        [&nextId]() { return new Connection(nextId++); },
        [](Connection* p) { delete p; },
        3
    );

    std::cout << "Initial pool size: " << pool.size() << "\n\n";

    // Acquire objects
    auto conn1 = pool.acquire();
    auto conn2 = pool.acquire();
    std::cout << "After acquiring 2: pool size = " << pool.size() << "\n";

    conn1->query("SELECT * FROM users");
    conn2->query("SELECT * FROM posts");

    // Release back to pool
    conn1.reset();
    conn2.reset();
    std::cout << "After releasing: pool size = " << pool.size() << "\n";

    // Acquire again (reuses objects)
    auto conn3 = pool.acquire();
    std::cout << "Reused connection id: " << conn3->id << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
