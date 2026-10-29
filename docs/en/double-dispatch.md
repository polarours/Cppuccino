# Double Dispatch

## What Is the Double Dispatch?

The Double Dispatch is a technique that lets one operation depend on the
runtime types of **two** objects. A C++ virtual call dispatches on the
runtime type of one object only — the object you call it on. Double dispatch
performs that selection twice: once for the first operand, once for the
second one.

> **Core idea**: turn the runtime type of the second operand into a
> compile-time overload by letting the first operand hand the operation over
> to the second one.

## Why Do We Need It?

### Problem Scenario

Suppose you write one round of rock-paper-scissors. The winner depends on the
runtime types of *both* moves:

```cpp
// Problem: the outcome depends on the types of both moves, but a virtual
// call can only dispatch on one of them.
std::string whoWins(Move& a, Move& b) {
    if (typeid(a) == typeid(Rock)) {
        if (typeid(b) == typeid(Paper)) {
            return "Paper beats Rock";
        }
        if (typeid(b) == typeid(Scissors)) {
            return "Rock beats Scissors";
        }
        return "a draw";
    }
    // ... the same nesting for Paper and Scissors
}
```

**Problems**:

1. The nesting grows with the number of moves — nine branches for three moves
2. The runtime types are compared as tags, so the compiler cannot check them
3. Every new move means editing every branch

### Solution

Let a visitor carry the operation, and dispatch twice — first on the type of
the first move, then on the type of the second one:

```cpp
// The visitor interface: one overload per move type.
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Rock& rock) = 0;
    virtual void visit(Paper& paper) = 0;
    virtual void visit(Scissors& scissors) = 0;
};

// The move interface: accept() hands the visitor a reference of our own
// type. That is the first half of the double dispatch.
class Element {
public:
    virtual ~Element() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string name() const = 0;
};

// The first dispatch lands in the overload of the first move. Inside it the
// second dispatch asks the opponent to accept a visitor that already knows
// our type, so the operation finally sees both types.
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
```

**Benefits**:

1. The operation lives in one place, not inside the move classes
2. Both runtime types are known where the outcome is computed
3. A new operation means a new visitor, not an edit of the moves

## Pattern Structure

```
                   ┌────────────────────────┐
                   │        Visitor         │
                   │  one overload per type │
                   └───────────┬────────────┘
                               │
                   ┌───────────▼────────────┐
                   │      MatchVisitor      │  ← first dispatch
                   │   (holds the opponent) │
                   └───────────┬────────────┘
                               │  opponent_.accept(...)
                   ┌───────────▼────────────┐
                   │ OpponentVisitor<First> │  ← second dispatch
                   └───────────┬────────────┘
                               │
          ┌────────────────────┼────────────────────┐
          │                    │                    │
   ┌──────▼──────┐      ┌──────▼──────┐      ┌──────▼──────┐
   │    Rock     │      │    Paper    │      │   Scissors  │
   └─────────────┘      └─────────────┘      └─────────────┘
                   ← the visited classes
```

## Implementation

### 1. Define the Element and Visitor Interfaces

```cpp
#include <iostream>
#include <string>

// Forward declarations: the visitor names every move type.
class Rock;
class Paper;
class Scissors;

// The visitor interface: one overload per move type. The overload is
// chosen by the runtime type of the argument.
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(Rock& rock) = 0;
    virtual void visit(Paper& paper) = 0;
    virtual void visit(Scissors& scissors) = 0;
};

// The move interface: accept() hands the visitor a reference of our own
// type, which is the first half of the double dispatch.
class Element {
public:
    virtual ~Element() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string name() const = 0;
};
```

### 2. Implement the Concrete Moves

```cpp
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
```

### 3. Write the Operation as an Overload Set

```cpp
// The operation: who wins a round. The overloads are picked by the two
// static types, which the double dispatch has already narrowed down.
std::string outcome(const Rock&, const Rock&) { return "a draw"; }
std::string outcome(const Rock&, const Paper&) { return "Paper beats Rock"; }
std::string outcome(const Rock&, const Scissors&) { return "Rock beats Scissors"; }
std::string outcome(const Paper&, const Rock&) { return "Paper beats Rock"; }
std::string outcome(const Paper&, const Paper&) { return "a draw"; }
std::string outcome(const Paper&, const Scissors&) { return "Scissors beats Paper"; }
std::string outcome(const Scissors&, const Rock&) { return "Scissors beats Rock"; }
std::string outcome(const Scissors&, const Paper&) { return "Scissors beats Paper"; }
std::string outcome(const Scissors&, const Scissors&) { return "a draw"; }
```

### 4. Implement the Two Visitors

```cpp
// The visitor for the second dispatch. It is templated on the type of the
// first move, so that inside every overload both types are known and the
// right outcome() overload can be selected.
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

// The visitor for the first dispatch. It holds the opponent and asks it to
// accept the second visitor.
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
```

### 5. Put the Two Dispatches Together

```cpp
// One round: the first dispatch picks the overload for the first move, the
// second dispatch picks the overload for the second move.
std::string playRound(Element& a, Element& b) {
    MatchVisitor visitor(b);
    a.accept(visitor);
    return visitor.result();
}

int main() {
    Rock rock;
    Paper paper;
    Scissors scissors;

    std::cout << "Rock vs Paper:    " << playRound(rock, paper) << "\n";
    std::cout << "Rock vs Scissors: " << playRound(rock, scissors) << "\n";
    std::cout << "Paper vs Rock:    " << playRound(paper, rock) << "\n";
    std::cout << "Paper vs Paper:   " << playRound(paper, paper) << "\n";
    std::cout << "Scissors vs Rock: " << playRound(scissors, rock) << "\n";
    std::cout << "Scissors vs Paper:" << playRound(scissors, paper) << "\n";

    return 0;
}
```

## Modern C++ Implementations

### Using std::variant and std::visit

```cpp
#include <variant>

// C++17: std::visit performs the same double dispatch for you. The
// overload set is picked by the two alternative types of the variants.
using Move = std::variant<Rock, Paper, Scissors>;

std::string playRoundVariant(const Move& a, const Move& b) {
    return std::visit(
        [](const auto& first, const auto& second) {
            return outcome(first, second);
        },
        a, b);
}
```

### Using if constexpr

```cpp
#include <type_traits>

// C++17: when both types are known at compile time, the compiler can do
// the dispatch on its own and no visitor is needed at all.
template<typename First, typename Second>
std::string outcomeIfConstexpr(const First&, const Second&) {
    if constexpr (std::is_same_v<First, Rock> && std::is_same_v<Second, Paper>) {
        return "Paper beats Rock";
    } else if constexpr (std::is_same_v<First, Paper> &&
                         std::is_same_v<Second, Scissors>) {
        return "Scissors beats Paper";
    } else {
        return "a draw";
    }
}
```

### Using a Type Object

```cpp
#include <string>
#include <vector>

// The relation between the moves can also live in data: one entry per
// move instead of one entry per pair of moves.
struct MoveType {
    std::string name;
    std::string beats;
};

const std::vector<MoveType> moveTypes = {
    {"Rock", "Scissors"},
    {"Paper", "Rock"},
    {"Scissors", "Paper"},
};

std::string outcomeByName(const std::string& a, const std::string& b) {
    for (const auto& type : moveTypes) {
        if (type.name != a) {
            continue;
        }
        if (a == b) {
            return "a draw";
        }
        return type.beats == b ? a + " beats " + b : b + " beats " + a;
    }
    return "unknown";
}
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Visitor** | The visitor dispatches on one type; double dispatch needs the types of two operands |
| **Strategy** | A strategy picks one algorithm for one object; double dispatch picks a behaviour from a pair |
| **Type Object** | A type object replaces the class hierarchy with data; double dispatch keeps the hierarchy and dispatches twice |
| **Overloading** | Overloading resolves on static types; double dispatch resolves on runtime types |

## Best Practices

### 1. Keep the Visited Classes Small

```cpp
// The move classes carry their own data and nothing else. Every operation
// lives in a visitor, so a new operation never touches them.
class Rock : public Element {
public:
    void accept(Visitor& visitor) override { visitor.visit(*this); }
    std::string name() const override { return "Rock"; }
};
```

### 2. Keep the Pair Table Next to the Operation

```cpp
// The table belongs to the operation, not to the move classes.
std::string outcome(const Rock&, const Scissors&) { return "Rock beats Scissors"; }
std::string outcome(const Scissors&, const Rock&) { return "Scissors beats Rock"; }
```

### 3. Give Every Visitor One Job

```cpp
// A second operation is a second visitor. The moves are not touched.
class CountVisitor : public Visitor {
public:
    void visit(Rock&) override { ++count_; }
    void visit(Paper&) override { ++count_; }
    void visit(Scissors&) override { ++count_; }

    int count() const { return count_; }

private:
    int count_ = 0;
};
```

## Common Traps

### Trap 1: One Dispatch Doing the Work of Two

```cpp
// Not recommended: the visitor only ever sees the first move, so the
// result is wrong for every pair but the first.
class BrokenVisitor : public Visitor {
public:
    void visit(Rock&) override { result_ = "Rock wins"; }
    void visit(Paper&) override { result_ = "Paper wins"; }
    void visit(Scissors&) override { result_ = "Scissors wins"; }

    const std::string& result() const { return result_; }

private:
    std::string result_;
};
```

### Trap 2: Forgetting the Second Dispatch

```cpp
// Not recommended: the opponent is never asked, so the second move is
// silently replaced by the first one and the table answers the wrong entry.
class OneSidedVisitor : public Visitor {
public:
    void visit(Rock& rock) override { result_ = outcome(rock, rock); }
    void visit(Paper& paper) override { result_ = outcome(paper, paper); }
    void visit(Scissors& scissors) override { result_ = outcome(scissors, scissors); }

    const std::string& result() const { return result_; }

private:
    std::string result_;
};
```

### Trap 3: Deleting Through a Non-Virtual Base

```cpp
// Not recommended: the visitor base has no virtual destructor, so a
// visitor that is deleted through a base pointer is destroyed wrongly.
class VisitorWithoutDestructor {
public:
    virtual void visit(Rock& rock) = 0;
    virtual void visit(Paper& paper) = 0;
    virtual void visit(Scissors& scissors) = 0;
};

// Recommended: give the base class a virtual destructor.
class VisitorWithDestructor {
public:
    virtual ~VisitorWithDestructor() = default;
    virtual void visit(Rock& rock) = 0;
    virtual void visit(Paper& paper) = 0;
    virtual void visit(Scissors& scissors) = 0;
};
```

## Example

A runnable version of this topic:

- [double-dispatch-demo.cpp](../../examples/double-dispatch-demo.cpp)

## Summary

The key points of double dispatch:

- **When to use it**: an operation depends on the runtime types of two
  objects, and a single virtual call is not enough
- **Main benefits**: both runtime types are known where the operation runs,
  the operation stays out of the visited classes
- **Implementation advice**: dispatch twice — the accept() of the first
  operand, then the accept() of the second one — and keep the pair table
  next to the operation
- **Common traps**: one dispatch doing the work of two, forgetting the
  second dispatch, deleting a visitor through a base without a virtual
  destructor

> **Remember**: the key of double dispatch is *dispatching twice* — the
> first operand chooses the visitor overload, and the second operand chooses
> the operation.

## See Also

- **Related docs:** [Visitor Pattern](visitor-pattern.md), [Strategy Pattern](strategy-pattern.md), [Type Object Pattern](type-object-pattern.md)
- **Pitfalls:** [Object Slicing](../../pitfalls/en/object-slicing.md), [Deleting Through Non-Virtual Base](../../pitfalls/en/deleting-through-non-virtual-base.md)
- **Examples:** [double-dispatch-demo.cpp](../../examples/double-dispatch-demo.cpp)
