#include "virtual_constructor.hpp"

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
    auto original = std::make_unique<virtual_constructor::Circle>(5.0);
    auto cloned = original->clone();

    expect(cloned->type() == "Circle", "expected Circle");
    expect(cloned->area() == original->area(), "expected same area");
    expect(original.get() != cloned.get(), "expected different pointers");
}

void test_clone_rectangle() {
    auto original = std::make_unique<virtual_constructor::Rectangle>(4.0, 6.0);
    auto cloned = original->clone();

    expect(cloned->type() == "Rectangle", "expected Rectangle");
    expect(cloned->area() == 24.0, "expected area 24");
}

void test_factory() {
    auto circle = virtual_constructor::ShapeFactory::create(
        virtual_constructor::ShapeFactory::ShapeType::Circle, 5.0);
    expect(circle->type() == "Circle", "expected Circle");

    auto rect = virtual_constructor::ShapeFactory::create(
        virtual_constructor::ShapeFactory::ShapeType::Rectangle, 4.0, 6.0);
    expect(rect->type() == "Rectangle", "expected Rectangle");
}

} // namespace

int main() {
    try {
        std::cout << "Virtual Constructor Tests:\n";
        test_clone_circle();
        std::cout << "  test_clone_circle: PASS\n";

        test_clone_rectangle();
        std::cout << "  test_clone_rectangle: PASS\n";

        test_factory();
        std::cout << "  test_factory: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "virtual_constructor_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nvirtual_constructor_tests passed" << std::endl;
    return 0;
}
