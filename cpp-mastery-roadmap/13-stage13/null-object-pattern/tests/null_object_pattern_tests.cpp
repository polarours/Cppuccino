#include "null_object_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_known_animal() {
    auto dog = null_object_pattern::AnimalFactory::create("dog");
    expect(dog->speak() == "Woof!", "expected Woof!");
    expect(dog->move() == "Runs", "expected Runs");
}

void test_unknown_animal() {
    auto unknown = null_object_pattern::AnimalFactory::create("unknown");
    expect(unknown->speak() == "(silence)", "expected silence");
    expect(unknown->move() == "(nothing)", "expected nothing");
}

void test_null_object_safety() {
    auto null = null_object_pattern::AnimalFactory::create("nonexistent");

    // Should not crash
    std::string sound = null->speak();
    std::string action = null->move();

    expect(!sound.empty(), "expected non-empty sound");
    expect(!action.empty(), "expected non-empty action");
}

void test_multiple_animals() {
    std::vector<std::string> types = {"dog", "cat", "bird", "unknown"};

    for (const auto& type : types) {
        auto animal = null_object_pattern::AnimalFactory::create(type);
        expect(!animal->speak().empty(), "expected non-empty speak");
    }
}

} // namespace

int main() {
    try {
        std::cout << "Null Object Pattern Tests:\n";
        test_known_animal();
        std::cout << "  test_known_animal: PASS\n";

        test_unknown_animal();
        std::cout << "  test_unknown_animal: PASS\n";

        test_null_object_safety();
        std::cout << "  test_null_object_safety: PASS\n";

        test_multiple_animals();
        std::cout << "  test_multiple_animals: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "null_object_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nnull_object_pattern_tests passed" << std::endl;
    return 0;
}
