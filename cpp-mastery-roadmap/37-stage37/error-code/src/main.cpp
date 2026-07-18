#include "error_code.hpp"

#include <iostream>

int main() {
    std::cout << "=== Error Code Demo ===\n\n";

    error_code::UserService service;
    service.addUser(1, "Alice");

    auto r1 = service.deleteUser(1);
    std::cout << "Delete 1: " << (r1.ok() ? "OK" : "FAIL") << "\n";

    auto r2 = service.deleteUser(99);
    std::cout << "Delete 99: " << r2.message() << "\n";

    auto r3 = service.deleteUser(-1);
    std::cout << "Delete -1: " << r3.message() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
