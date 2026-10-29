#include "visitor_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// The classes under test report what they do by printing, so the output
// is what the test looks at.
class CapturedOutput {
public:
    CapturedOutput() : previous_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CapturedOutput() { std::cout.rdbuf(previous_); }

    CapturedOutput(const CapturedOutput&) = delete;
    CapturedOutput& operator=(const CapturedOutput&) = delete;

    std::string text() const { return buffer_.str(); }

private:
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

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

    CapturedOutput output;
    circle.accept(printer);
    rect.accept(printer);
    triangle.accept(printer);

    const std::string text = output.text();
    expect(text.find("Circle with radius 5") != std::string::npos,
           "expected the circle to be printed, got: " + text);
    expect(text.find("Rectangle 4x6") != std::string::npos,
           "expected the rectangle to be printed, got: " + text);
    expect(text.find("Triangle base=3 height=8") != std::string::npos,
           "expected the triangle to be printed, got: " + text);
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

    } catch (const std::exception& exception) {
        std::cerr << "visitor_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nvisitor_pattern_tests passed" << std::endl;
    return 0;
}