#pragma once

#include <string>
#include <iostream>
#include <memory>

namespace double_dispatch {

// --- Forward declarations ---

class Rock;
class Paper;
class Scissors;

// --- Visitor Interface ---

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Rock& rock) = 0;
    virtual void visit(Paper& paper) = 0;
    virtual void visit(Scissors& scissors) = 0;
};

// --- Element Interface ---

class Element {
public:
    virtual ~Element() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string name() const = 0;
};

// --- Concrete Elements ---

class Rock : public Element {
public:
    void accept(Visitor& visitor) override { visitor.visit(*this); }
    std::string name() const override { return "Rock"; }
};

class Paper : public Element {
public:
    void accept(Visitor& visitor) override { visitor.visit(*this); }
    std::string name() const override { return "Paper"; }
};

class Scissors : public Element {
public:
    void accept(Visitor& visitor) override { visitor.visit(*this); }
    std::string name() const override { return "Scissors"; }
};

// --- Game Visitor ---

class GameVisitor : public Visitor {
public:
    void visit(Rock& rock) override {
        result_ = "Rock crushes Scissors";
    }

    void visit(Paper& paper) override {
        result_ = "Paper covers Rock";
    }

    void visit(Scissors& scissors) override {
        result_ = "Scissors cuts Paper";
    }

    std::string getResult() const { return result_; }

private:
    std::string result_;
};

// --- Double Dispatch Function ---

std::string playGame(Element& a, Element& b) {
    class GameVisitor visitor;
    a.accept(visitor);
    std::string resultA = visitor.getResult();

    b.accept(visitor);
    std::string resultB = visitor.getResult();

    return resultA + " and " + resultB;
}

} // namespace double_dispatch
