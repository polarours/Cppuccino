#include "repository_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== Repository Pattern Demo ===\n\n";

    repository_pattern::UserRepository repo;
    repo.save({1, "Alice", "alice@test.com"});
    repo.save({2, "Bob", "bob@test.com"});

    auto user = repo.findById(1);
    if (user) {
        std::cout << "Found: " << user->name() << "\n";
    }

    auto alices = repo.findByPredicate([](const repository_pattern::User& u) {
        return u.name().find("Alice") != std::string::npos;
    });
    std::cout << "Alice count: " << alices.size() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
