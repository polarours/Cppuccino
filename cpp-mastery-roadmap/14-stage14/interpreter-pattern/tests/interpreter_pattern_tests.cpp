#include "interpreter_pattern.hpp"

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

void test_number_expression() {
    auto num = std::make_shared<interpreter_pattern::NumberExpression>(42);
    expect(num->interpret() == 42, "expected 42");
    expect(num->toString() == "42", "expected 42");
}

void test_add_expression() {
    auto left = std::make_shared<interpreter_pattern::NumberExpression>(3);
    auto right = std::make_shared<interpreter_pattern::NumberExpression>(4);
    auto add = std::make_shared<interpreter_pattern::AddExpression>(left, right);

    expect(add->interpret() == 7, "expected 7");
    expect(add->toString() == "(3 + 4)", "expected (3 + 4)");
}

void test_subtract_expression() {
    auto left = std::make_shared<interpreter_pattern::NumberExpression>(10);
    auto right = std::make_shared<interpreter_pattern::NumberExpression>(3);
    auto sub = std::make_shared<interpreter_pattern::SubtractExpression>(left, right);

    expect(sub->interpret() == 7, "expected 7");
    expect(sub->toString() == "(10 - 3)", "expected (10 - 3)");
}

void test_multiply_expression() {
    auto left = std::make_shared<interpreter_pattern::NumberExpression>(4);
    auto right = std::make_shared<interpreter_pattern::NumberExpression>(5);
    auto mul = std::make_shared<interpreter_pattern::MultiplyExpression>(left, right);

    expect(mul->interpret() == 20, "expected 20");
    expect(mul->toString() == "(4 * 5)", "expected (4 * 5)");
}

void test_nested_expression() {
    // (3 + 4) * 2
    auto add = std::make_shared<interpreter_pattern::AddExpression>(
        std::make_shared<interpreter_pattern::NumberExpression>(3),
        std::make_shared<interpreter_pattern::NumberExpression>(4)
    );
    auto mul = std::make_shared<interpreter_pattern::MultiplyExpression>(
        add,
        std::make_shared<interpreter_pattern::NumberExpression>(2)
    );

    expect(mul->interpret() == 14, "expected 14");
    expect(mul->toString() == "((3 + 4) * 2)", "expected ((3 + 4) * 2)");
}

void test_variable_expression() {
    interpreter_pattern::VariableExpression::clearContext();
    interpreter_pattern::VariableExpression::setContext("x", 10);
    interpreter_pattern::VariableExpression::setContext("y", 5);

    auto x = std::make_shared<interpreter_pattern::VariableExpression>("x");
    auto y = std::make_shared<interpreter_pattern::VariableExpression>("y");
    auto add = std::make_shared<interpreter_pattern::AddExpression>(x, y);

    expect(add->interpret() == 15, "expected 15");
}

} // namespace

int main() {
    try {
        std::cout << "Interpreter Pattern Tests:\n";
        test_number_expression();
        std::cout << "  test_number_expression: PASS\n";

        test_add_expression();
        std::cout << "  test_add_expression: PASS\n";

        test_subtract_expression();
        std::cout << "  test_subtract_expression: PASS\n";

        test_multiply_expression();
        std::cout << "  test_multiply_expression: PASS\n";

        test_nested_expression();
        std::cout << "  test_nested_expression: PASS\n";

        test_variable_expression();
        std::cout << "  test_variable_expression: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "interpreter_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ninterpreter_pattern_tests passed" << std::endl;
    return 0;
}
