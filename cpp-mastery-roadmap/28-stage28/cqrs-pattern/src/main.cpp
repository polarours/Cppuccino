#include "cqrs_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== CQRS Pattern Demo ===\n\n";

    cqrs_pattern::AccountService service;

    service.commands().dispatch("create", "Alice");
    service.commands().dispatch("create", "Bob");
    service.commands().dispatch("deposit", "1:1000");
    service.commands().dispatch("deposit", "2:500");

    std::cout << "Alice: " << service.queries().query("get", "1") << "\n";
    std::cout << "Bob: " << service.queries().query("get", "2") << "\n";
    std::cout << "All: " << service.queries().query("list", "") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
