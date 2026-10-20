# Decorator Pattern

## What Is the Decorator Pattern?

The Decorator Pattern is a structural design pattern that lets you attach new
behavior to an existing object without changing its structure. It creates a
decorator class that wraps the original class and provides extra behavior.

> **Core idea**: extend objects through composition rather than inheritance,
> keeping each class single-purpose.

## Why Do We Need It?

### Problem Scenario

Suppose we have a graphics system that must draw all kinds of shapes:

```cpp
// Problem: inheritance causes class explosion
class Shape {
public:
    virtual void draw() = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape { /* ... */ };
class Rectangle : public Shape { /* ... */ };

// If we need red circles, blue circles, green circles...
class RedCircle : public Circle { /* ... */ };
class BlueCircle : public Circle { /* ... */ };
class RedRectangle : public Rectangle { /* ... */ };
class BlueRectangle : public Rectangle { /* ... */ };
// number of classes = shapes × colors × other attributes...
```

**Problems**:

1. Inheritance makes the class count explode
2. Hard to add features dynamically
3. Violates the open-closed principle

### Solution

Use the decorator pattern:

```cpp
#include <memory>
#include <iostream>

class ShapeDecorator : public Shape {
public:
    explicit ShapeDecorator(std::unique_ptr<Shape> shape)
        : shape_(std::move(shape)) {}

    void draw() override { shape_->draw(); }

protected:
    std::unique_ptr<Shape> shape_;
};

class RedDecorator : public ShapeDecorator {
public:
    using ShapeDecorator::ShapeDecorator;

    void draw() override {
        std::cout << "[Red] ";
        shape_->draw();
    }
};

class BlueDecorator : public ShapeDecorator {
public:
    using ShapeDecorator::ShapeDecorator;

    void draw() override {
        std::cout << "[Blue] ";
        shape_->draw();
    }
};
```

**Benefits**:

1. Flexible composition of features
2. Original classes stay untouched
3. Complies with the open-closed principle

## Pattern Structure

```
                    ┌─────────────┐
                    │   Component │
                    │  (Shape)    │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │   Circle    │ │ Rectangle   │ │  Triangle   │ ← concrete component
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │     Decorator           │ ← abstract decorator
              │   (ShapeDecorator)      │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  RedBorder  │ │  Shadow     │ │  Glow       │ ← concrete decorators
    └─────────────┘ └─────────────┘ └─────────────┘
```

## Implementation

### 1. Define the Component Interface

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>

class Coffee {
public:
    virtual ~Coffee() = default;
    virtual std::string describe() const = 0;
    virtual double cost() const = 0;
};
```

### 2. Implement the Concrete Components

```cpp
class SimpleCoffee : public Coffee {
public:
    std::string describe() const override {
        return "Simple Coffee";
    }

    double cost() const override {
        return 5.0;
    }
};

class Espresso : public Coffee {
public:
    std::string describe() const override {
        return "Espresso";
    }

    double cost() const override {
        return 8.0;
    }
};

class Latte : public Coffee {
public:
    std::string describe() const override {
        return "Latte";
    }

    double cost() const override {
        return 10.0;
    }
};
```

### 3. Define the Decorator

```cpp
class CoffeeDecorator : public Coffee {
public:
    explicit CoffeeDecorator(std::unique_ptr<Coffee> coffee)
        : coffee_(std::move(coffee)) {}

    std::string describe() const override {
        return coffee_->describe();
    }

    double cost() const override {
        return coffee_->cost();
    }

protected:
    std::unique_ptr<Coffee> coffee_;
};
```

### 4. Implement the Concrete Decorators

```cpp
class MilkDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;

    std::string describe() const override {
        return coffee_->describe() + " with Milk";
    }

    double cost() const override {
        return coffee_->cost() + 2.0;
    }
};

class SugarDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;

    std::string describe() const override {
        return coffee_->describe() + " with Sugar";
    }

    double cost() const override {
        return coffee_->cost() + 1.0;
    }
};

class WhippedCreamDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;

    std::string describe() const override {
        return coffee_->describe() + " with Whipped Cream";
    }

    double cost() const override {
        return coffee_->cost() + 3.0;
    }
};

class CaramelDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;

    std::string describe() const override {
        return coffee_->describe() + " with Caramel";
    }

    double cost() const override {
        return coffee_->cost() + 2.5;
    }
};
```

### 5. Client Usage

```cpp
int main() {
    // plain coffee — declare as unique_ptr<Coffee>, not auto:
    // auto would deduce unique_ptr<SimpleCoffee> and later assignments of
    // MilkDecorator etc. would not compile
    std::unique_ptr<Coffee> coffee = std::make_unique<SimpleCoffee>();
    std::cout << coffee->describe() << " - $" << coffee->cost() << "\n";

    // with milk
    coffee = std::make_unique<MilkDecorator>(std::make_unique<SimpleCoffee>());
    std::cout << coffee->describe() << " - $" << coffee->cost() << "\n";

    // with milk and sugar
    coffee = std::make_unique<SugarDecorator>(
        std::make_unique<MilkDecorator>(std::make_unique<SimpleCoffee>())
    );
    std::cout << coffee->describe() << " - $" << coffee->cost() << "\n";

    // latte with whipped cream and caramel
    coffee = std::make_unique<CaramelDecorator>(
        std::make_unique<WhippedCreamDecorator>(
            std::make_unique<MilkDecorator>(
                std::make_unique<Latte>()
            )
        )
    );
    std::cout << coffee->describe() << " - $" << coffee->cost() << "\n";

    return 0;
}
```

## Modern C++ Implementations

### With a Template Decorator

```cpp
template <typename T>
class TemplatedDecorator : public T {
public:
    using T::T;  // inherit constructors

    std::string describe() const {
        return static_cast<const T&>(*this).describe() + " (decorated)";
    }
};
```

### With Fluent Chaining

```cpp
class DecoratedCoffee {
public:
    explicit DecoratedCoffee(std::unique_ptr<Coffee> coffee)
        : coffee_(std::move(coffee)) {}

    DecoratedCoffee& addMilk() {
        coffee_ = std::make_unique<MilkDecorator>(std::move(coffee_));
        return *this;
    }

    DecoratedCoffee& addSugar() {
        coffee_ = std::make_unique<SugarDecorator>(std::move(coffee_));
        return *this;
    }

    DecoratedCoffee& addWhippedCream() {
        coffee_ = std::make_unique<WhippedCreamDecorator>(std::move(coffee_));
        return *this;
    }

    DecoratedCoffee& addCaramel() {
        coffee_ = std::make_unique<CaramelDecorator>(std::move(coffee_));
        return *this;
    }

    std::string describe() const { return coffee_->describe(); }
    double cost() const { return coffee_->cost(); }

private:
    std::unique_ptr<Coffee> coffee_;
};

void demo() {
    // Note: the chained methods return a reference to *this — bind it to a
    // named object, not to `auto` (which would try to copy the
    // non-copyable DecoratedCoffee from a dangling reference).
    auto coffee = DecoratedCoffee(std::make_unique<Latte>());
    coffee.addMilk().addSugar().addWhippedCream().addCaramel();
    std::cout << coffee.describe() << " - $" << coffee.cost() << "\n";
}
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Inheritance** | Inheritance adds behavior at compile time; decorators add it at runtime |
| **Proxy** | Proxy controls access; a decorator adds behavior |
| **Composite** | Composite handles tree structures; a decorator wraps a single object |

## Best Practices

### 1. Keep Decorators Transparent

```cpp
class TransparentDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;

    // every part of the interface is forwarded to the wrapped object
    std::string describe() const override {
        return coffee_->describe();
    }

    double cost() const override {
        return coffee_->cost();
    }
};
```

### 2. Use Multi-Layer Decoration

```cpp
void demoLayers() {
    // layers stack up
    auto coffee = std::make_unique<SugarDecorator>(
        std::make_unique<MilkDecorator>(
            std::make_unique<WhippedCreamDecorator>(
                std::make_unique<SimpleCoffee>()
            )
        )
    );
    std::cout << coffee->describe() << " - $" << coffee->cost() << "\n";
}
```

### 3. Avoid Over-Decoration

```cpp
// Not recommended: too many layers (each of A/B/C/D/E would have to be its
// own class — the nesting gets unreadable fast)
// auto coffee = std::make_unique<SugarDecorator>(
//     std::make_unique<MilkDecorator>(
//         std::make_unique<WhippedCreamDecorator>(
//             std::make_unique<SimpleCoffee>()
//         )
//     )
// );

// Recommended: one decorator that bundles the extras
class SuperDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;

    std::string describe() const override {
        return coffee_->describe() + " (with everything)";
    }

    double cost() const override {
        return coffee_->cost() + 10.0;
    }
};
```

## Common Traps

### Trap 1: Decorators Holding Each Other

```cpp
#include <memory>

// Danger: two decorators holding one another — with shared_ptr this is a
// reference cycle and a guaranteed leak; unique_ptr at least refuses to
// compile one, which is your friend.
class ADecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;

    void setOther(std::unique_ptr<CoffeeDecorator> other) {
        other_ = std::move(other);
    }

    std::string describe() const override {
        return coffee_->describe() + " + " +
               (other_ ? other_->describe() : std::string{});
    }

private:
    std::unique_ptr<CoffeeDecorator> other_;
};
```

### Trap 2: Forgetting to Manage Resources

```cpp
// Not recommended: raw-pointer management
class BadDecorator {
public:
    explicit BadDecorator(Coffee* coffee) : coffee_(coffee) {}

private:
    Coffee* coffee_;  // no ownership — can dangle
};

// Recommended: smart pointers
class GoodDecorator {
public:
    explicit GoodDecorator(std::unique_ptr<Coffee> coffee)
        : coffee_(std::move(coffee)) {}

private:
    std::unique_ptr<Coffee> coffee_;  // clear ownership
};
```

### Trap 3: Inconsistent State

```cpp
// Problem: the decorator and the wrapped object fall out of sync
class StatefulDecorator : public CoffeeDecorator {
public:
    using CoffeeDecorator::CoffeeDecorator;

    void setSpecialOption(bool option) {
        option_ = option;
        // forgot to propagate to coffee_
    }

private:
    bool option_ = false;
};
```

## Summary

The key points of the decorator pattern:

- **When to use it**: dynamically add behavior, avoid class explosion, support
  composition
- **Main benefits**: flexible, extensible, open-closed friendly
- **Implementation advice**: keep decorators transparent, allow layers, use
  smart pointers
- **Common traps**: reference cycles, resource leaks, inconsistent state

> **Remember**: the key of the decorator pattern is *transparent wrapping* —
> a decorator should be usable exactly like the object it wraps.
