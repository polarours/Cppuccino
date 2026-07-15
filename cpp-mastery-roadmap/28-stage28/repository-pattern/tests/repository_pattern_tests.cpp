#include "repository_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_save_and_find() {
    repository_pattern::UserRepository repo;
    repo.save({1, "Alice", "alice@test.com"});

    auto user = repo.findById(1);
    expect(user != nullptr, "expected user found");
    expect(user->name() == "Alice", "expected Alice");
}

void test_find_all() {
    repository_pattern::UserRepository repo;
    repo.save({1, "Alice", "alice@test.com"});
    repo.save({2, "Bob", "bob@test.com"});

    auto all = repo.findAll();
    expect(all.size() == 2, "expected 2 users");
}

void test_remove() {
    repository_pattern::UserRepository repo;
    repo.save({1, "Alice", "alice@test.com"});
    repo.remove(1);

    expect(repo.findById(1) == nullptr, "expected null after remove");
    expect(repo.count() == 0, "expected 0 count");
}

void test_find_by_predicate() {
    repository_pattern::UserRepository repo;
    repo.save({1, "Alice", "alice@test.com"});
    repo.save({2, "Bob", "bob@test.com"});
    repo.save({3, "Alice II", "alice2@test.com"});

    auto alices = repo.findByPredicate([](const repository_pattern::User& u) {
        return u.name().find("Alice") != std::string::npos;
    });
    expect(alices.size() == 2, "expected 2 Alices");
}

void test_exists() {
    repository_pattern::UserRepository repo;
    repo.save({1, "Alice", "alice@test.com"});

    expect(repo.exists(1), "expected exists");
    expect(!repo.exists(99), "expected not exists");
}

} // namespace

int main() {
    try {
        std::cout << "Repository Pattern Tests:\n";
        test_save_and_find();
        std::cout << "  test_save_and_find: PASS\n";

        test_find_all();
        std::cout << "  test_find_all: PASS\n";

        test_remove();
        std::cout << "  test_remove: PASS\n";

        test_find_by_predicate();
        std::cout << "  test_find_by_predicate: PASS\n";

        test_exists();
        std::cout << "  test_exists: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "repository_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nrepository_pattern_tests passed" << std::endl;
    return 0;
}
