// examples/double-dispatch-demo.cpp
// Demonstrates Double Dispatch: one operation that depends on the runtime types of two operands.
// Compile with: g++ -std=c++17 -o double_dispatch_demo double-dispatch-demo.cpp

#include <iostream>
#include <string>
#include <variant>

// --- Forward declarations ---

class Rock;
class Paper;
class Scissors;

// --- Visitor interface: one overload per move type ---

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Rock& rock) = 0;
    virtual void visit(Paper& paper) = 0;
    virtual void visit(Scissors& scissors) = 0;
};

// --- Element interface: accept() hands the visitor a reference of our type ---

class Element {
public:
    virtual ~Element() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string name() const = 0;
};

// --- Concrete elements ---

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

// --- The operation: an overload set picked by the two static types ---

std::string outcome(const Rock&, const Rock&) { return "a draw"; }
std::string outcome(const Rock&, const Paper&) { return "Paper beats Rock"; }
std::string outcome(const Rock&, const Scissors&) { return "Rock beats Scissors"; }
std::string outcome(const Paper&, const Rock&) { return "Paper beats Rock"; }
std::string outcome(const Paper&, const Paper&) { return "a draw"; }
std::string outcome(const Paper&, const Scissors&) { return "Scissors beats Paper"; }
std::string outcome(const Scissors&, const Rock&) { return "Scissors beats Rock"; }
std::string outcome(const Scissors&, const Paper&) { return "Scissors beats Paper"; }
std::string outcome(const Scissors&, const Scissors&) { return "a draw"; }

// --- The second dispatch: a visitor templated on the type of the first move ---

template<typename First>
class OpponentVisitor : public Visitor {
public:
    explicit OpponentVisitor(First& first) : first_(first) {}

    void visit(Rock& second) override { result_ = outcome(first_, second); }
    void visit(Paper& second) override { result_ = outcome(first_, second); }
    void visit(Scissors& second) override { result_ = outcome(first_, second); }

    const std::string& result() const { return result_; }

private:
    First& first_;
    std::string result_;
};

// --- The first dispatch: a visitor that holds the opponent ---

class MatchVisitor : public Visitor {
public:
    explicit MatchVisitor(Element& opponent) : opponent_(opponent) {}

    void visit(Rock& rock) override {
        OpponentVisitor<Rock> visitor(rock);
        opponent_.accept(visitor);
        result_ = visitor.result();
    }

    void visit(Paper& paper) override {
        OpponentVisitor<Paper> visitor(paper);
        opponent_.accept(visitor);
        result_ = visitor.result();
    }

    void visit(Scissors& scissors) override {
        OpponentVisitor<Scissors> visitor(scissors);
        opponent_.accept(visitor);
        result_ = visitor.result();
    }

    const std::string& result() const { return result_; }

private:
    Element& opponent_;
    std::string result_;
};

// --- One round: two dispatches ---

std::string playRound(Element& a, Element& b) {
    MatchVisitor visitor(b);
    a.accept(visitor);
    return visitor.result();
}

// --- The same rule with std::visit ---

using Move = std::variant<Rock, Paper, Scissors>;

std::string playRoundVariant(const Move& a, const Move& b) {
    return std::visit(
        [](const auto& first, const auto& second) {
            return outcome(first, second);
        },
        a, b);
}

// --- A second operation: a new visitor, the moves are not touched ---

class DescribeVisitor : public Visitor {
public:
    void visit(Rock&) override { description_ = "a hard stone"; }
    void visit(Paper&) override { description_ = "a flat sheet"; }
    void visit(Scissors&) override { description_ = "two sharp blades"; }

    const std::string& description() const { return description_; }

private:
    std::string description_;
};

int main() {
    Rock rock;
    Paper paper;
    Scissors scissors;

    std::cout << "=== Double Dispatch Demo ===\n\n";

    std::cout << "--- One round, two dispatches ---\n";
    std::cout << "Rock vs Paper:     " << playRound(rock, paper) << "\n";
    std::cout << "Rock vs Scissors:  " << playRound(rock, scissors) << "\n";
    std::cout << "Paper vs Rock:     " << playRound(paper, rock) << "\n";
    std::cout << "Paper vs Paper:    " << playRound(paper, paper) << "\n";
    std::cout << "Scissors vs Rock:  " << playRound(scissors, rock) << "\n";
    std::cout << "Scissors vs Paper: " << playRound(scissors, paper) << "\n";

    std::cout << "\n--- The same rule with std::visit ---\n";
    Move first = Rock{};
    Move second = Paper{};
    std::cout << "Rock vs Paper:     " << playRoundVariant(first, second) << "\n";

    std::cout << "\n--- A new operation, the same moves ---\n";
    DescribeVisitor describe;
    rock.accept(describe);
    std::cout << "Rock:              " << describe.description() << "\n";
    scissors.accept(describe);
    std::cout << "Scissors:          " << describe.description() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
