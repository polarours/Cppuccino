// examples/interpreter-pattern-demo.cpp
// Demonstrates Interpreter Pattern: Expression tree with Number/Add/Multiply/Variable.
// Compile: g++ -std=c++20 -o interpreter-pattern-demo interpreter-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace interpreter_pattern {

class Expression {
public:
    virtual ~Expression() = default;
    virtual int interpret() const = 0;
    virtual std::string toString() const = 0;
};

class NumberExpression : public Expression {
public:
    explicit NumberExpression(int value) : value_(value) {}
    int interpret() const override { return value_; }
    std::string toString() const override { return std::to_string(value_); }
private:
    int value_;
};

class VariableExpression : public Expression {
public:
    explicit VariableExpression(const std::string& name) : name_(name) {}

    int interpret() const override {
        auto it = context_.find(name_);
        return (it != context_.end()) ? it->second : 0;
    }

    std::string toString() const override { return name_; }

    static void setContext(const std::string& var, int value) {
        context_[var] = value;
    }

    static void clearContext() { context_.clear(); }

private:
    std::string name_;
    static std::map<std::string, int> context_;
};

std::map<std::string, int> VariableExpression::context_;

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::shared_ptr<Expression> left,
                     std::shared_ptr<Expression> right,
                     char op) : left_(std::move(left)), right_(std::move(right)), op_(op) {}

    int interpret() const override {
        int a = left_->interpret();
        int b = right_->interpret();
        switch (op_) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return b != 0 ? a / b : 0;
            default: return 0;
        }
    }

    std::string toString() const override {
        return "(" + left_->toString() + " " + op_ + " " + right_->toString() + ")";
    }

private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
    char op_;
};

} // namespace interpreter_pattern

int main() {
    using namespace interpreter_pattern;

    std::cout << "=== Interpreter Pattern Demo ===\n\n";

    // Simple expression: 3 + 4
    auto add = std::make_shared<BinaryExpression>(
        std::make_shared<NumberExpression>(3),
        std::make_shared<NumberExpression>(4),
        '+'
    );
    std::cout << "(3 + 4) = " << add->interpret()
              << "  [" << add->toString() << "]\n";

    // Nested expression: (3 + 4) * 2
    auto mul = std::make_shared<BinaryExpression>(
        add,
        std::make_shared<NumberExpression>(2),
        '*'
    );
    std::cout << "(3 + 4) * 2 = " << mul->interpret()
              << "  [" << mul->toString() << "]\n";

    // With variables: x + y where x=10, y=5
    VariableExpression::setContext("x", 10);
    VariableExpression::setContext("y", 5);

    auto varAdd = std::make_shared<BinaryExpression>(
        std::make_shared<VariableExpression>("x"),
        std::make_shared<VariableExpression>("y"),
        '+'
    );
    std::cout << "x + y = " << varAdd->interpret()
              << "  [" << varAdd->toString() << "]\n";

    VariableExpression::clearContext();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
