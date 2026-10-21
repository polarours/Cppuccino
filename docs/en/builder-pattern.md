# Builder Pattern

## What Is the Builder Pattern?

The Builder Pattern is a creational design pattern that separates the
construction of a complex object from its representation, so that the same
construction process can create different representations.

> **Core idea**: encapsulate the construction steps of an object inside a
> builder, creating complex objects through step-by-step assembly.

## Why Do We Need It?

### Problem Scenario

Suppose we have a `Pizza` object with many optional parameters:

```cpp
// Problem: too many constructor parameters
class Pizza {
public:
    Pizza(bool cheese, bool pepperoni, bool mushrooms, 
          bool olives, bool onions, bool peppers,
          bool sausage, bool ham, bool bacon,
          double size, bool thinCrust, bool stuffedCrust)
        : cheese_(cheese), pepperoni_(pepperoni), ... {}
    
private:
    bool cheese_;
    bool pepperoni_;
    // ... 10+ parameters
};

// Hard to read at the call site
Pizza p(true, false, true, false, true, false, false, false, false, 14, true, false);
```

**Problems**:

1. Too many parameters make the constructor hard to use
2. The parameter order is easy to mix up
3. Parameter validity is hard to verify

### Solution

Use the builder pattern:

```cpp
class PizzaBuilder {
public:
    PizzaBuilder& size(double size) { size_ = size; return *this; }
    PizzaBuilder& thinCrust() { thinCrust_ = true; return *this; }
    PizzaBuilder& addCheese() { cheese_ = true; return *this; }
    PizzaBuilder& addPepperoni() { pepperoni_ = true; return *this; }
    // ... other methods
    
    Pizza build() const {
        validate();
        return Pizza(size_, thinCrust_, cheese_, pepperoni_, ...);
    }

private:
    double size_ = 12.0;
    bool thinCrust_ = false;
    bool cheese_ = false;
    bool pepperoni_ = false;
    // ...
};

// Clear and readable at the call site
Pizza p = PizzaBuilder()
    .size(14)
    .thinCrust()
    .addCheese()
    .addPepperoni()
    .build();
```

**Benefits**:

1. Readable: each method call expresses the intent clearly
2. Verifiable: the parameters can be validated in `build()`
3. Flexible: objects with different configurations can be created
4. Supports immutable objects: the object cannot be modified after `build()`

## Pattern Structure

```
                    ┌─────────────┐
                    │  Director   │
                    │   (director)│
                    └──────┬──────┘
                           │ uses
                           │
              ┌────────────▼────────────┐
              │       Builder           │ ← abstract builder
              │   (PizzaBuilder)        │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Hawaiian   │ │  Vegetarian │ │  MeatLover  │ ← concrete builders
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │        Product          │ ← the product
              │        (Pizza)          │
              └─────────────────────────┘
```

## Implementation

### 1. Define the Product

```cpp
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

class Pizza {
public:
    Pizza(double size, bool thinCrust, bool stuffedCrust,
          bool cheese, bool tomatoSauce,
          std::vector<std::string> toppings)
        : size_(size), thinCrust_(thinCrust), stuffedCrust_(stuffedCrust),
          cheese_(cheese), tomatoSauce_(tomatoSauce),
          toppings_(std::move(toppings)) {}
    
    void display() const {
        std::cout << "=== Pizza ===\n";
        std::cout << "Size: " << size_ << " inches\n";
        std::cout << "Crust: " << (thinCrust_ ? "Thin" : "Regular");
        if (stuffedCrust_) std::cout << " (Stuffed)";
        std::cout << "\n";
        std::cout << "Cheese: " << (cheese_ ? "Yes" : "No") << "\n";
        std::cout << "Sauce: " << (tomatoSauce_ ? "Tomato" : "None") << "\n";
        std::cout << "Toppings:\n";
        for (const auto& t : toppings_) {
            std::cout << "  - " << t << "\n";
        }
    }

private:
    double size_;
    bool thinCrust_;
    bool stuffedCrust_;
    bool cheese_;
    bool tomatoSauce_;
    std::vector<std::string> toppings_;
};
```

### 2. Define the Abstract Builder

```cpp
class PizzaBuilder {
public:
    virtual ~PizzaBuilder() = default;
    virtual PizzaBuilder& size(double size) = 0;
    virtual PizzaBuilder& thinCrust() = 0;
    virtual PizzaBuilder& stuffedCrust() = 0;
    virtual PizzaBuilder& cheese() = 0;
    virtual PizzaBuilder& tomatoSauce() = 0;
    virtual PizzaBuilder& addTopping(const std::string& topping) = 0;
    virtual Pizza build() const = 0;
};
```

### 3. Implement the Concrete Builders

```cpp
class HawaiianPizzaBuilder : public PizzaBuilder {
public:
    PizzaBuilder& size(double size) override {
        size_ = size;
        return *this;
    }
    
    PizzaBuilder& thinCrust() override {
        thinCrust_ = true;
        return *this;
    }
    
    PizzaBuilder& stuffedCrust() override {
        stuffedCrust_ = true;
        return *this;
    }
    
    PizzaBuilder& cheese() override {
        cheese_ = true;
        return *this;
    }
    
    PizzaBuilder& tomatoSauce() override {
        sauce_ = "Tomato";
        return *this;
    }
    
    PizzaBuilder& addTopping(const std::string& topping) override {
        toppings_.push_back(topping);
        return *this;
    }
    
    Pizza build() const override {
        if (toppings_.empty()) {
            throw std::invalid_argument("Hawaiian pizza must have toppings");
        }
        return Pizza(size_, thinCrust_, stuffedCrust_, cheese_, 
                     sauce_ == "Tomato", toppings_);
    }

private:
    double size_ = 12.0;
    bool thinCrust_ = false;
    bool stuffedCrust_ = false;
    bool cheese_ = true;
    std::string sauce_ = "None";
    std::vector<std::string> toppings_;
};

class VegetarianPizzaBuilder : public PizzaBuilder {
public:
    PizzaBuilder& size(double size) override { size_ = size; return *this; }
    PizzaBuilder& thinCrust() override { thinCrust_ = true; return *this; }
    PizzaBuilder& stuffedCrust() override { stuffedCrust_ = true; return *this; }
    PizzaBuilder& cheese() override { cheese_ = true; return *this; }
    PizzaBuilder& tomatoSauce() override { sauce_ = "Tomato"; return *this; }
    PizzaBuilder& addTopping(const std::string& topping) override {
        toppings_.push_back(topping);
        return *this;
    }
    
    Pizza build() const override {
        if (toppings_.empty()) {
            throw std::invalid_argument("Vegetarian pizza must have toppings");
        }
        return Pizza(size_, thinCrust_, stuffedCrust_, cheese_,
                     sauce_ == "Tomato", toppings_);
    }

private:
    double size_ = 12.0;
    bool thinCrust_ = false;
    bool stuffedCrust_ = false;
    bool cheese_ = true;
    std::string sauce_ = "None";
    std::vector<std::string> toppings_;
};
```

### 4. Implement the Director Class

```cpp
class Waiter {
public:
    Pizza construct(PizzaBuilder& builder) {
        // the director controls the construction process
        return builder.size(14)
                    .thinCrust()
                    .cheese()
                    .tomatoSauce()
                    .addTopping("Peppers")
                    .addTopping("Onions")
                    .build();
    }
};
```

### 5. Client Usage

```cpp
int main() {
    // created directly
    HawaiianPizzaBuilder hawaiian;
    auto pizza1 = hawaiian.size(14)
                          .thinCrust()
                          .cheese()
                          .addTopping("Ham")
                          .addTopping("Pineapple")
                          .build();
    pizza1.display();
    
    // created through the director
    Waiter waiter;
    VegetarianPizzaBuilder veggie;
    auto pizza2 = waiter.construct(veggie);
    pizza2.display();
    
    return 0;
}
```

## Modern C++ Implementations

### With a Fluent API

```cpp
class ModernPizzaBuilder {
public:
    ModernPizzaBuilder& size(double s) { size_ = s; return *this; }
    ModernPizzaBuilder& thinCrust() { thinCrust_ = true; return *this; }
    ModernPizzaBuilder& stuffedCrust() { stuffedCrust_ = true; return *this; }
    ModernPizzaBuilder& cheese() { cheese_ = true; return *this; }
    ModernPizzaBuilder& sauce(const std::string& s) { sauce_ = s; return *this; }
    ModernPizzaBuilder& topping(const std::string& t) { 
        toppings_.push_back(t); return *this; 
    }
    
    Pizza build() const {
        return Pizza(size_, thinCrust_, stuffedCrust_, cheese_, 
                     sauce_ == "Tomato", toppings_);
    }

private:
    double size_ = 12.0;
    bool thinCrust_ = false;
    bool stuffedCrust_ = false;
    bool cheese_ = false;
    std::string sauce_ = "None";
    std::vector<std::string> toppings_;
};
```

### With Static Factory Methods

```cpp
class PizzaBuilder {
public:
    // static factory method
    static ModernPizzaBuilder hawaiian() {
        return ModernPizzaBuilder()
            .cheese()
            .sauce("Tomato")
            .topping("Ham")
            .topping("Pineapple");
    }
    
    static ModernPizzaBuilder vegetarian() {
        return ModernPizzaBuilder()
            .cheese()
            .sauce("Tomato")
            .topping("Peppers")
            .topping("Onions")
            .topping("Mushrooms");
    }
    
    static ModernPizzaBuilder meatLover() {
        return ModernPizzaBuilder()
            .cheese()
            .sauce("Tomato")
            .topping("Pepperoni")
            .topping("Sausage")
            .topping("Bacon");
    }
};

// usage
auto pizza = PizzaBuilder::hawaiian().size(14).thinCrust().build();
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Factory Method** | The factory creates a complete object; the builder assembles it step by step |
| **Abstract Factory** | The abstract factory creates a family of products; the builder focuses on assembling a single object |
| **Prototype** | The prototype creates by cloning; the builder creates through steps |

## Best Practices

### 1. Use Immutable Objects

```cpp
class Pizza {
public:
    // every member is const
    const double size_;
    const bool thinCrust_;
    // ...
    
private:
    Pizza(...) = default;  // private constructor
    friend class PizzaBuilder;  // only the builder may create it
};
```

### 2. Validate Parameters

```cpp
Pizza build() const {
    if (size_ < 8 || size_ > 24) {
        throw std::invalid_argument("Invalid size: " + std::to_string(size_));
    }
    if (toppings_.empty()) {
        throw std::invalid_argument("Pizza must have at least one topping");
    }
    return Pizza(size_, thinCrust_, stuffedCrust_, cheese_, 
                 sauce_ == "Tomato", toppings_);
}
```

### 3. Support Optional Parameters

```cpp
class PizzaBuilder {
public:
    PizzaBuilder& size(double s) { size_ = s; return *this; }
    PizzaBuilder& thinCrust() { thinCrust_ = true; return *this; }
    // optional parameters
    PizzaBuilder& extraCheese() { extraCheese_ = true; return *this; }
    PizzaBuilder& garlicButter() { garlicButter_ = true; return *this; }
    
    // with a default value
    PizzaBuilder& sauce(const std::string& s = "Tomato") { 
        sauce_ = s; return *this; 
    }
};
```

## Common Traps

### Trap 1: The Builder Is Too Complex

```cpp
// Not recommended: the builder contains too much logic
class ComplexBuilder {
public:
    PizzaBuilder& step1() { /* complex logic */ return *this; }
    PizzaBuilder& step2() { /* complex logic */ return *this; }
    // ... 50+ steps
};

// Recommended: keep it simple
class SimpleBuilder {
public:
    PizzaBuilder& size(double s) { size_ = s; return *this; }
    PizzaBuilder& topping(const std::string& t) { 
        toppings_.push_back(t); return *this; 
    }
};
```

### Trap 2: Forgetting to Call build()

```cpp
// Not recommended: forgetting build()
PizzaBuilder builder;
builder.size(14).cheese();  // forgot build()!

// Recommended: use a scope or RAII
auto pizza = PizzaBuilder{}.size(14).cheese().build();
```

### Trap 3: Leaking Builder State

```cpp
// Not recommended: the builder state can be modified
class BadBuilder {
public:
    void setCheese(bool c) { cheese_ = c; }  // exposes the internal state
    
private:
    bool cheese_ = false;
};

// Recommended: expose only the building interface
class GoodBuilder {
public:
    GoodBuilder& cheese() { cheese_ = true; return *this; }
    
private:
    bool cheese_ = false;
};
```

## Summary

The key points of the builder pattern:

- **When to use it**: the object has many parameters, the construction
  process is complex, or several representations are needed
- **Main benefits**: readable, verifiable, supports immutable objects
- **Implementation advice**: use a fluent API, validate the parameters,
  support optional parameters
- **Common traps**: too much complexity, forgetting `build()`, leaking state

> **Remember**: the key of the builder pattern is *"step-by-step construction"*
> — break the construction of a complex object into several simple steps so
> the process stays clear and controllable.
