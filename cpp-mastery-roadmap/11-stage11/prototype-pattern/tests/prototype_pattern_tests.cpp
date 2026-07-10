#include "prototype_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_clone_circle() {
    auto original = std::make_unique<prototype_pattern::Circle>(5.0);
    auto cloned = original->clone();

    expect(cloned->type() == "Circle", "expected Circle");
    expect(cloned->area() == original->area(), "expected same area");
    expect(original.get() != cloned.get(), "expected different pointers");
}

void test_clone_rectangle() {
    auto original = std::make_unique<prototype_pattern::Rectangle>(4.0, 6.0);
    auto cloned = original->clone();

    expect(cloned->type() == "Rectangle", "expected Rectangle");
    expect(cloned->area() == 24.0, "expected area 24");
}

void test_registry() {
    prototype_pattern::ShapeRegistry registry;

    registry.registerShape("circle", std::make_shared<prototype_pattern::Circle>(3.0));
    registry.registerShape("rectangle", std::make_shared<prototype_pattern::Rectangle>(2.0, 4.0));

    auto circle = registry.create("circle");
    expect(circle->type() == "Circle", "expected Circle");

    auto rect = registry.create("rectangle");
    expect(rect->type() == "Rectangle", "expected Rectangle");
}

void test_registry_unknown() {
    prototype_pattern::ShapeRegistry registry;

    bool threw = false;
    try {
        registry.create("unknown");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "expected exception for unknown shape");
}

void test_clone_independence() {
    auto original = std::make_unique<prototype_pattern::Circle>(5.0);
    auto cloned = original->clone();

    // Modifying original doesn't affect clone
    auto another = original->clone();
    expect(original.get() != cloned.get(), "expected different pointers");
    expect(original.get() != another.get(), "expected different pointers");
}

} // namespace

int main() {
    try {
        std::cout << "Prototype Pattern Tests:\n";
        test_clone_circle();
        std::cout << "  test_clone_circle: PASS\n";

        test_clone_rectangle();
        std::cout << "  test_clone_rectangle: PASS\n";

        test_registry();
        std::cout << "  test_registry: PASS\n";

        test_registry_unknown();
        std::cout << "  test_registry_unknown: PASS\n";

        test_clone_independence();
        std::cout << "  test_clone_independence: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "prototype_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nprototype_pattern_tests passed" << std::endl;
    return 0;
}
