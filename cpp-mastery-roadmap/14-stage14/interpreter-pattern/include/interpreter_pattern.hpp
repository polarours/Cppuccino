#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace interpreter_pattern {

// --- Expression Interface ---

class Expression {
public:
    virtual ~Expression() = default;
    virtual int interpret() const = 0;
    virtual std::string toString() const = 0;
};

// --- Terminal Expressions ---

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

    static void setContext(const std::string& name, int value) {
        context_[name] = value;
    }

    static void clearContext() {
        context_.clear();
    }

private:
    std::string name_;
    static inline std::map<std::string, int> context_;
};

// --- Non-Terminal Expressions ---

class AddExpression : public Expression {
public:
    AddExpression(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    int interpret() const override {
        return left_->interpret() + right_->interpret();
    }

    std::string toString() const override {
        return "(" + left_->toString() + " + " + right_->toString() + ")";
    }

private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};

class SubtractExpression : public Expression {
public:
    SubtractExpression(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    int interpret() const override {
        return left_->interpret() - right_->interpret();
    }

    std::string toString() const override {
        return "(" + left_->toString() + " - " + right_->toString() + ")";
    }

private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};

class MultiplyExpression : public Expression {
public:
    MultiplyExpression(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    int interpret() const override {
        return left_->interpret() * right_->interpret();
    }

    std::string toString() const override {
        return "(" + left_->toString() + " * " + right_->toString() + ")";
    }

private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};

} // namespace interpreter_pattern
