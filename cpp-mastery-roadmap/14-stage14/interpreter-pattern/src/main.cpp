#include "interpreter_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Interpreter Pattern Demo ===\n\n";

    // Create expression: (3 + 4) * 2
    auto add = std::make_shared<interpreter_pattern::AddExpression>(
        std::make_shared<interpreter_pattern::NumberExpression>(3),
        std::make_shared<interpreter_pattern::NumberExpression>(4)
    );
    auto mul = std::make_shared<interpreter_pattern::MultiplyExpression>(
        add,
        std::make_shared<interpreter_pattern::NumberExpression>(2)
    );

    std::cout << "Expression: " << mul->toString() << "\n";
    std::cout << "Result: " << mul->interpret() << "\n\n";

    // With variables
    interpreter_pattern::VariableExpression::setContext("x", 10);
    interpreter_pattern::VariableExpression::setContext("y", 5);

    auto expr = std::make_shared<interpreter_pattern::AddExpression>(
        std::make_shared<interpreter_pattern::VariableExpression>("x"),
        std::make_shared<interpreter_pattern::VariableExpression>("y")
    );

    std::cout << "With variables: " << expr->toString() << " = " << expr->interpret() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
