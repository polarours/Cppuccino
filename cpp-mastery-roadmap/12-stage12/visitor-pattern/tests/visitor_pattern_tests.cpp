#include "visitor_pattern.hpp"

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

void test_area_calculator() {
    visitor_pattern::Circle circle(5.0);
    visitor_pattern::AreaCalculator calculator;

    circle.accept(calculator);
    expect(calculator.getResult() == 3.14159 * 25, "expected circle area");
}

void test_printer() {
    visitor_pattern::Circle circle(5.0);
    visitor_pattern::Rectangle rect(4.0, 6.0);
    visitor_pattern::Triangle triangle(3.0, 8.0);

    visitor_pattern::Printer printer;
    circle.accept(printer);
    rect.accept(printer);
    triangle.accept(printer);
}

void test_multiple_shapes() {
    std::vector<std::unique_ptr<visitor_pattern::Shape>> shapes;
    shapes.push_back(std::make_unique<visitor_pattern::Circle>(5.0));
    shapes.push_back(std::make_unique<visitor_pattern::Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<visitor_pattern::Triangle>(3.0, 8.0));

    visitor_pattern::AreaCalculator calculator;
    double totalArea = 0;

    for (auto& shape : shapes) {
        shape->accept(calculator);
        totalArea += calculator.getResult();
    }

    expect(totalArea > 0, "expected positive total area");
}

} // namespace

int main() {
    try {
        std::cout << "Visitor Pattern Tests:\n";
        test_area_calculator();
        std::cout << "  test_area_calculator: PASS\n";

        test_printer();
        std::cout << "  test_printer: PASS\n";

        test_multiple_shapes();
        std::cout << "  test_multiple_shapes: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "visitor_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nvisitor_pattern_tests passed" << std::endl;
    return 0;
}