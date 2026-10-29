# Interpreter Pattern

## What Is the Interpreter Pattern?

The Interpreter Pattern is a behavioral design pattern that defines how to
evaluate a sentence written in a small language. Every rule of the grammar
becomes a class, and the program evaluates a sentence by walking a tree of
those classes.

> **Core idea**: one class per grammar rule — the expression tree evaluates
> itself by recursion.

## Why Do We Need It?

### Problem Scenario

Suppose you are writing a small expression language for a rule engine. The
first version has three operations, and it is easy to write:

```cpp
// Problem: one class carries parsing, evaluation and printing
class ExpressionEvaluator {
public:
    int evaluate(const std::string& expression) const {
        // every new operation means a new branch here, another one in
        // describe(), and another one in the parser
        if (expression == "1 + 2") {
            return 3;
        }
        if (expression == "3 * 4") {
            return 12;
        }
        if (expression == "x + 1") {
            return 10 + 1;
        }
        return 0;
    }

    std::string describe(const std::string& expression) const {
        if (expression == "1 + 2") {
            return "(1 + 2)";
        }
        if (expression == "3 * 4") {
            return "(3 * 4)";
        }
        return expression;
    }
};
```

**Problems**:

1. Every new operation adds a branch to every function
2. The evaluator, the printer and the parser drift apart
3. The rules cannot be composed — "1 + 2" cannot be nested inside another rule

### Solution

Use the interpreter pattern: give every rule of the grammar its own class,
and let the classes evaluate themselves recursively.

```cpp
// The context holds the variables that the expression reads
class Context;

// The abstract expression: one operation, the same for every node
class Expression {
public:
    virtual ~Expression() = default;
    virtual int interpret(const Context& context) const = 0;
    virtual std::string toString() const = 0;
};

// A concrete expression: the sum of two expressions
class AddExpression : public Expression {
public:
    AddExpression(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    int interpret(const Context& context) const override {
        return left_->interpret(context) + right_->interpret(context);
    }

    std::string toString() const override {
        return "(" + left_->toString() + " + " + right_->toString() + ")";
    }

private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};
```

**Benefits**:

1. The grammar becomes a class hierarchy, one class per rule
2. A new operation is a new class, not a new branch in an existing function
3. The evaluation is recursive and uniform: the client only calls `interpret()`

## Pattern Structure

```
                    ┌──────────────────┐
                    │      Client      │
                    └────────┬─────────┘
                             │ builds
                    ┌────────▼─────────┐
                    │     Expression   │ ← abstract expression
                    └────────┬─────────┘
                             │
           ┌─────────────────┼─────────────────┐
           │                 │                 │
  ┌────────▼────────┐ ┌──────▼───────┐ ┌───────▼────────┐
  │ Number          │ │ Variable     │ │ Add            │
  │ (terminal)      │ │ (terminal)   │ │ (composite)    │
  └─────────────────┘ └──────────────┘ └───────┬────────┘
                                                 │ children
                                        ┌────────▼────────┐
                                        │    Expression   │
                                        └─────────────────┘
```

```
┌──────────┐   the variables   ┌──────────────────────────┐
│ Context  │ ────────────────► │  Expression::interpret() │
└──────────┘                   └────────────┬─────────────┘
                                             │ recurses into the children
                                   ┌─────────▼─────────┐
                                   │    Expression     │
                                   └───────────────────┘
```

## Implementation

### 1. Define the Expression Interface and the Context

```cpp
#include <memory>
#include <string>
#include <map>
#include <iostream>

// The context holds the variables that the expression reads
class Context {
public:
    void set(const std::string& name, int value) {
        variables_[name] = value;
    }

    int get(const std::string& name) const {
        auto it = variables_.find(name);
        return it != variables_.end() ? it->second : 0;
    }

private:
    std::map<std::string, int> variables_;
};

// The abstract expression: one operation, the same for every node
class Expression {
public:
    virtual ~Expression() = default;
    virtual int interpret(const Context& context) const = 0;
    virtual std::string toString() const = 0;
};
```

### 2. Implement the Terminal Expressions

```cpp
// A leaf: a literal number
class NumberExpression : public Expression {
public:
    explicit NumberExpression(int value) : value_(value) {}

    int interpret(const Context& context) const override {
        (void)context;  // a number ignores the context
        return value_;
    }

    std::string toString() const override {
        return std::to_string(value_);
    }

private:
    int value_;
};

// A leaf: a variable that is looked up in the context
class VariableExpression : public Expression {
public:
    explicit VariableExpression(std::string name) : name_(std::move(name)) {}

    int interpret(const Context& context) const override {
        return context.get(name_);
    }

    std::string toString() const override {
        return name_;
    }

private:
    std::string name_;
};
```

### 3. Implement the Non-Terminal Expressions

```cpp
// A composite: the sum of two expressions
class AddExpression : public Expression {
public:
    AddExpression(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    int interpret(const Context& context) const override {
        return left_->interpret(context) + right_->interpret(context);
    }

    std::string toString() const override {
        return "(" + left_->toString() + " + " + right_->toString() + ")";
    }

private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};

// A composite: the product of two expressions
class MultiplyExpression : public Expression {
public:
    MultiplyExpression(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    int interpret(const Context& context) const override {
        return left_->interpret(context) * right_->interpret(context);
    }

    std::string toString() const override {
        return "(" + left_->toString() + " * " + right_->toString() + ")";
    }

private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};
```

### 4. Build and Evaluate an Expression Tree

```cpp
int main() {
    // The tree for (3 + 4) * 2
    auto add = std::make_shared<AddExpression>(
        std::make_shared<NumberExpression>(3),
        std::make_shared<NumberExpression>(4)
    );
    auto tree = std::make_shared<MultiplyExpression>(
        add,
        std::make_shared<NumberExpression>(2)
    );

    std::cout << "Expression: " << tree->toString() << "\n";
    std::cout << "Result: " << tree->interpret(Context{}) << "\n";

    // The tree for x + y, evaluated with a context
    Context context;
    context.set("x", 10);
    context.set("y", 5);

    auto sum = std::make_shared<AddExpression>(
        std::make_shared<VariableExpression>("x"),
        std::make_shared<VariableExpression>("y")
    );

    std::cout << "With variables: " << sum->toString()
              << " = " << sum->interpret(context) << "\n";

    return 0;
}
```

## Modern C++ Implementations

### Using std::variant and std::visit

```cpp
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

struct Add;
struct Multiply;

// A node is a number, a variable name, or a composite node
using Expression = std::variant<int,
                                std::string,
                                std::shared_ptr<Add>,
                                std::shared_ptr<Multiply>>;

struct Add {
    Expression left;
    Expression right;
};

struct Multiply {
    Expression left;
    Expression right;
};

int interpret(const Expression& expression, const std::map<std::string, int>& variables);

int interpret(const Expression& expression, const std::map<std::string, int>& variables) {
    return std::visit([&](const auto& node) -> int {
        using Node = std::decay_t<decltype(node)>;

        if constexpr (std::is_same_v<Node, int>) {
            return node;
        } else if constexpr (std::is_same_v<Node, std::string>) {
            auto it = variables.find(node);
            return it != variables.end() ? it->second : 0;
        } else if constexpr (std::is_same_v<Node, std::shared_ptr<Add>>) {
            return interpret(node->left, variables) + interpret(node->right, variables);
        } else {
            return interpret(node->left, variables) * interpret(node->right, variables);
        }
    }, expression);
}
```

### Using Templates for the Operators

```cpp
// One template replaces every binary operation
template<typename Op>
class BinaryExpression : public Expression {
public:
    BinaryExpression(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    int interpret(const Context& context) const override {
        return Op{}(left_->interpret(context), right_->interpret(context));
    }

    std::string toString() const override {
        return "(" + left_->toString() + " " + Op::symbol() + " " + right_->toString() + ")";
    }

private:
    std::shared_ptr<Expression> left_;
    std::shared_ptr<Expression> right_;
};

struct AddOp {
    int operator()(int a, int b) const { return a + b; }
    static const char* symbol() { return "+"; }
};

struct MultiplyOp {
    int operator()(int a, int b) const { return a * b; }
    static const char* symbol() { return "*"; }
};

// The tree is now built with
// std::make_shared<BinaryExpression<AddOp>>(left, right)
```

### Erasing the Node Type with std::function

```cpp
#include <functional>

// The node type is erased: the tree becomes a tree of callables
using Node = std::function<int(const Context&)>;

Node number(int value) {
    return [value](const Context&) { return value; };
}

Node variable(std::string name) {
    return [name = std::move(name)](const Context& context) {
        return context.get(name);
    };
}

Node add(Node left, Node right) {
    return [left = std::move(left), right = std::move(right)](const Context& context) {
        return left(context) + right(context);
    };
}
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Strategy** | Strategy picks one algorithm at run time; the interpreter defines a small language and evaluates it recursively |
| **Interpreter vs parser** | The parser builds the tree, the interpreter evaluates it; the two are often used together |
| **Interpreter vs visitor** | The visitor visits a structure that already exists; the interpreter defines the structure and the operations together |

## Best Practices

### 1. Keep the Grammar and the Hierarchy in Sync

```cpp
// The grammar of the language:
//
//   expression := term (('+' | '-') term)*
//   term       := factor (('*' | '/') factor)*
//   factor     := NUMBER | VARIABLE | '(' expression ')'
//
// One class per rule, and the hierarchy mirrors the grammar:
//
//   Expression
//   ├── NumberExpression      (factor)
//   ├── VariableExpression    (factor)
//   ├── AddExpression         (expression)
//   ├── SubtractExpression    (expression)
//   ├── MultiplyExpression    (term)
//   └── DivideExpression      (term)
```

### 2. Separate Parsing from Evaluation

```cpp
// The parser builds the tree, the interpreter evaluates it
class Parser {
public:
    explicit Parser(std::string text) : text_(std::move(text)) {}

    std::shared_ptr<Expression> parse() const {
        // a real parser tokenizes the text and builds the tree
        return std::make_shared<NumberExpression>(0);
    }

private:
    std::string text_;
};

// The client uses the two in turn
Context context;
context.set("x", 10);

auto tree = Parser("3 + 4 * 2").parse();
std::cout << tree->interpret(context) << "\n";
```

### 3. Let the Tree Own Its Children

```cpp
// Recommended: shared_ptr lets every node own its children
auto add = std::make_shared<AddExpression>(
    std::make_shared<NumberExpression>(3),
    std::make_shared<NumberExpression>(4)
);

auto tree = std::make_shared<MultiplyExpression>(
    add,
    std::make_shared<NumberExpression>(2)
);

// The same tree is evaluated with different contexts
Context context;
context.set("x", 10);

std::cout << tree->interpret(context) << "\n";
```

## Common Traps

### Trap 1: The Context Is Ignored

```cpp
// Not recommended: the node keeps its own copy of the value
class VariableExpression : public Expression {
public:
    explicit VariableExpression(std::string name, int value)
        : name_(std::move(name)), value_(value) {}

    int interpret(const Context& context) const override {
        (void)context;      // the context is ignored
        return value_;      // the value was frozen when the tree was built
    }

    std::string toString() const override {
        return name_;
    }

private:
    std::string name_;
    int value_;
};

// Recommended: read the value from the context at evaluation time
class VariableExpression : public Expression {
public:
    explicit VariableExpression(std::string name) : name_(std::move(name)) {}

    int interpret(const Context& context) const override {
        return context.get(name_);
    }

    std::string toString() const override {
        return name_;
    }

private:
    std::string name_;
};
```

### Trap 2: An Unknown Variable Silently Becomes Zero

```cpp
// Not recommended: a typo in a variable name is invisible
int get(const std::string& name) const {
    auto it = variables_.find(name);
    return it != variables_.end() ? it->second : 0;
}

// Recommended: report the missing variable
int get(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        throw std::runtime_error("unknown variable: " + name);
    }
    return it->second;
}
```

### Trap 3: The Tree Is Not Immutable

```cpp
// Not recommended: the evaluation changes the tree it walks
class AccumulateExpression : public Expression {
public:
    explicit AccumulateExpression(std::shared_ptr<Expression> left)
        : left_(std::move(left)) {}

    int interpret(const Context& context) const override {
        total_ += left_->interpret(context);  // the node keeps a running total
        return total_;
    }

    std::string toString() const override {
        return left_->toString();
    }

private:
    std::shared_ptr<Expression> left_;
    mutable int total_ = 0;
};

// Evaluating the same tree twice gives two different answers
```

## Example

A runnable version of this topic:

- [interpreter-pattern-demo.cpp](../../examples/interpreter-pattern-demo.cpp)

## Summary

The key points of the interpreter pattern:

- **When to use it**: a small language, a stable grammar, operations that the
  language itself defines
- **Main benefits**: the grammar becomes a class hierarchy, a new rule is a
  new class, the evaluation stays uniform
- **Implementation advice**: separate parsing from evaluation, keep the
  variables in one context, let shared_ptr own the tree
- **Common traps**: the context being ignored, an unknown variable silently
  becoming zero, a tree that is mutated during the evaluation

> **Remember**: the key of the interpreter pattern is *one class per grammar
> rule* — the expression tree evaluates itself by recursion.

## See Also

- **Related docs:** [Composite Pattern](composite-pattern.md), [Visitor Pattern](visitor-pattern.md), [Strategy Pattern](strategy-pattern.md)
- **Pitfalls:** [Memory Leak](../../pitfalls/en/memory-leak.md), [Object Slicing](../../pitfalls/en/object-slicing.md), [Stack Overflow](../../pitfalls/en/stack-overflow.md)
- **Examples:** [interpreter-pattern-demo.cpp](../../examples/interpreter-pattern-demo.cpp)
