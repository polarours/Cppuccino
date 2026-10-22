# Specification Pattern

## What Is the Specification Pattern?

The Specification Pattern is a behavioral design pattern that encapsulates
business rules as reusable specification objects, making rule validation and
composition more flexible and easier to maintain.

> **Core idea**: encapsulate the logic of "does this satisfy that condition?"
> in standalone specification objects that can be combined and reused.

## Why Do We Need It?

### Problem Scenario

Suppose we have an e-commerce system that has to validate orders:

```cpp
// Problem: validation logic is scattered around and hard to reuse
class Order {
public:
    bool isValid() const {
        return !items_.empty()
            && total_ > 0
            && customer_.hasValidAddress()
            && customer_.hasValidPayment();
    }

    bool canShip() const {
        return isValid()
            && total_ >= 100
            && customer_.isVip();
    }

private:
    std::vector<Item> items_;
    double total_ = 0;
    Customer customer_;
};
```

**Problems**:

1. Validation logic is hard-coded into the class
2. Rules are hard to reuse
3. Complex rules are hard to combine

### Solution

Use the specification pattern:

```cpp
// specification interface
template<typename T>
class Specification {
public:
    virtual ~Specification() = default;
    virtual bool isSatisfiedBy(const T& item) const = 0;
    virtual std::unique_ptr<Specification<T>> and_(
        const std::unique_ptr<Specification<T>>& other) const = 0;
    virtual std::unique_ptr<Specification<T>> or_(
        const std::unique_ptr<Specification<T>>& other) const = 0;
    virtual std::unique_ptr<Specification<T>> not_() const = 0;
};

// concrete specification
class TotalGreaterThanSpecification : public Specification<Order> {
public:
    explicit TotalGreaterThanSpecification(double amount) : amount_(amount) {}

    bool isSatisfiedBy(const Order& order) const override {
        return order.getTotal() > amount_;
    }

    std::unique_ptr<Specification<Order>> and_(
        const std::unique_ptr<Specification<Order>>& other) const override {
        return std::make_unique<AndSpecification<Order>>(
            std::unique_ptr<Specification<Order>>(this), other);
    }

    // ... or_ and not_ implementations
private:
    double amount_;
};

// usage
auto spec = std::make_unique<TotalGreaterThanSpecification>(100)
    ->and_(std::make_unique<CustomerIsVipSpecification>());

bool result = spec->isSatisfiedBy(order);
```

**Benefits**:

1. Rules are reusable
2. Rules are easy to combine
3. Complies with the open-closed principle

## Pattern Structure

```
                    ┌─────────────┐
                    │Specification│
                    │  (the rule)  │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │ AndSpec     │ │  OrSpec     │ │ NotSpec     │ ← composite specifications
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │    ConcreteSpecification│ ← concrete specification
              │   (TotalGreaterThan)    │
              └────────────┬────────────┘
                           │
                    ┌────────▼────────┐
                    │     Target      │ ← the object under test
                    └─────────────────┘
```

## Implementation

### 1. Define the Specification Interface

```cpp
#include <memory>
#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// The composite specifications are referenced from Specification<T>, so they
// have to be declared before it.
template<typename T> class AndSpecification;
template<typename T> class OrSpecification;
template<typename T> class NotSpecification;

template<typename T>
class Specification {
public:
    virtual ~Specification() = default;
    virtual bool isSatisfiedBy(const T& item) const = 0;

    // combination operations
    std::shared_ptr<Specification<T>> and_(
        const std::shared_ptr<const Specification<T>>& other) const {
        return std::make_shared<AndSpecification<T>>(getSelf(), other);
    }

    std::shared_ptr<Specification<T>> or_(
        const std::shared_ptr<const Specification<T>>& other) const {
        return std::make_shared<OrSpecification<T>>(getSelf(), other);
    }

    std::shared_ptr<Specification<T>> not_() const {
        return std::make_shared<NotSpecification<T>>(getSelf());
    }

protected:
    virtual std::shared_ptr<const Specification<T>> getSelf() const = 0;
};
```

### 2. Implement the Composite Specifications

```cpp
template<typename T>
class AndSpecification : public Specification<T>,
                          public std::enable_shared_from_this<AndSpecification<T>> {
public:
    AndSpecification(std::shared_ptr<const Specification<T>> left,
                     std::shared_ptr<const Specification<T>> right)
        : left_(left), right_(std::move(right)) {}

    bool isSatisfiedBy(const T& item) const override {
        bool result = left_->isSatisfiedBy(item) && right_->isSatisfiedBy(item);
        std::cout << "And: " << item << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<T>> getSelf() const override {
        // this-> is required: shared_from_this() comes from a dependent base.
        return this->shared_from_this();
    }

private:
    std::shared_ptr<const Specification<T>> left_;
    std::shared_ptr<const Specification<T>> right_;
};

template<typename T>
class OrSpecification : public Specification<T>,
                         public std::enable_shared_from_this<OrSpecification<T>> {
public:
    OrSpecification(std::shared_ptr<const Specification<T>> left,
                    std::shared_ptr<const Specification<T>> right)
        : left_(left), right_(std::move(right)) {}

    bool isSatisfiedBy(const T& item) const override {
        bool result = left_->isSatisfiedBy(item) || right_->isSatisfiedBy(item);
        std::cout << "Or: " << item << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<T>> getSelf() const override {
        // this-> is required: shared_from_this() comes from a dependent base.
        return this->shared_from_this();
    }

private:
    std::shared_ptr<const Specification<T>> left_;
    std::shared_ptr<const Specification<T>> right_;
};

template<typename T>
class NotSpecification : public Specification<T>,
                          public std::enable_shared_from_this<NotSpecification<T>> {
public:
    explicit NotSpecification(std::shared_ptr<const Specification<T>> spec)
        : spec_(spec) {}

    bool isSatisfiedBy(const T& item) const override {
        bool result = !spec_->isSatisfiedBy(item);
        std::cout << "Not: " << item << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<T>> getSelf() const override {
        // this-> is required: shared_from_this() comes from a dependent base.
        return this->shared_from_this();
    }

private:
    std::shared_ptr<const Specification<T>> spec_;
};
```

### 3. Implement the Concrete Specifications

```cpp
// specification on the order total
class TotalGreaterThanSpecification : public Specification<int>,
                                      public std::enable_shared_from_this<TotalGreaterThanSpecification> {
public:
    explicit TotalGreaterThanSpecification(int amount) : amount_(amount) {}

    bool isSatisfiedBy(const int& total) const override {
        bool result = total > amount_;
        std::cout << "Total > " << amount_ << ": " << total
                  << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<int>> getSelf() const override {
        // this-> is required: shared_from_this() comes from a dependent base.
        return this->shared_from_this();
    }

private:
    int amount_;
};

// specification on the number of items
class ItemCountSpecification : public Specification<int>,
                                public std::enable_shared_from_this<ItemCountSpecification> {
public:
    explicit ItemCountSpecification(int minCount, int maxCount)
        : minCount_(minCount), maxCount_(maxCount) {}

    bool isSatisfiedBy(const int& count) const override {
        bool result = count >= minCount_ && count <= maxCount_;
        std::cout << "Count [" << minCount_ << "," << maxCount_ << "]: "
                  << count << " -> " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<int>> getSelf() const override {
        // this-> is required: shared_from_this() comes from a dependent base.
        return this->shared_from_this();
    }

private:
    int minCount_;
    int maxCount_;
};

// blacklist specification
class NotInBlacklistSpecification : public Specification<std::string>,
                                     public std::enable_shared_from_this<NotInBlacklistSpecification> {
public:
    explicit NotInBlacklistSpecification(const std::vector<std::string>& blacklist)
        : blacklist_(blacklist) {}

    bool isSatisfiedBy(const std::string& customer) const override {
        bool result = std::find(blacklist_.begin(), blacklist_.end(), customer)
                      == blacklist_.end();
        std::cout << "Customer '" << customer << "' not in blacklist: "
                  << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

protected:
    std::shared_ptr<const Specification<std::string>> getSelf() const override {
        // this-> is required: shared_from_this() comes from a dependent base.
        return this->shared_from_this();
    }

private:
    std::vector<std::string> blacklist_;
};
```

### 4. Client Usage

```cpp
int main() {
    // create specifications
    auto totalSpec = std::make_shared<TotalGreaterThanSpecification>(100);
    auto countSpec = std::make_shared<ItemCountSpecification>(1, 10);
    auto blacklistSpec = std::make_shared<NotInBlacklistSpecification>(
        std::vector<std::string>{"bad_customer_1", "bad_customer_2"});

    // combine specifications — every spec in one chain must share the item type
    auto complexSpec = totalSpec->and_(countSpec->or_(totalSpec));

    // validate
    std::cout << "=== Testing ===\n";
    complexSpec->isSatisfiedBy(150);

    // simple specification combination
    auto simpleSpec = totalSpec->and_(countSpec);
    std::cout << "\n=== Simple Test ===\n";
    simpleSpec->isSatisfiedBy(50);

    // the blacklist spec works on a customer name, so it is checked on its own
    std::cout << "\n=== Blacklist Test ===\n";
    blacklistSpec->isSatisfiedBy("bad_customer_1");

    return 0;
}
```

## Modern C++ Implementations

### Simplifying with Lambdas

```cpp
class LambdaSpecification {
public:
    using Predicate = std::function<bool(const int&)>;

    LambdaSpecification(Predicate pred) : pred_(std::move(pred)) {}

    bool isSatisfiedBy(const int& item) const {
        return pred_(item);
    }

    std::unique_ptr<LambdaSpecification> and_(
        const std::unique_ptr<LambdaSpecification>& other) const {
        auto pred = [this, other](const int& item) {
            return pred_(item) && other->pred_(item);
        };
        return std::make_unique<LambdaSpecification>(std::move(pred));
    }

    Predicate pred_;
};
```

### Simplifying Composition with Templates

```cpp
template<typename T>
class CompositeSpecification : public Specification<T> {
public:
    CompositeSpecification(std::unique_ptr<Specification<T>> left,
                           std::unique_ptr<Specification<T>> right)
        : left_(std::move(left)), right_(std::move(right)) {}

protected:
    std::unique_ptr<Specification<T>> left_;
    std::unique_ptr<Specification<T>> right_;
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Strategy** | A strategy picks an algorithm; a specification validates a condition |
| **Command** | A command encapsulates a request; a specification encapsulates a condition |
| **Chain of Responsibility** | A chain handles requests in order; a specification combines validations |

## Best Practices

### 1. Create Specifications with a Factory

```cpp
class SpecificationFactory {
public:
    static std::shared_ptr<Specification<int>> createTotalSpecification(int amount) {
        return std::make_shared<TotalGreaterThanSpecification>(amount);
    }

    static std::shared_ptr<Specification<int>> createCountSpecification(
        int min, int max) {
        return std::make_shared<ItemCountSpecification>(min, max);
    }
};
```

### 2. Support Cached Results

```cpp
#include <map>

template<typename T>
class CachedSpecification : public Specification<T> {
public:
    explicit CachedSpecification(std::unique_ptr<Specification<T>> spec)
        : spec_(std::move(spec)) {}

    bool isSatisfiedBy(const T& item) const override {
        auto it = cache_.find(item);
        if (it != cache_.end()) {
            return it->second;
        }
        bool result = spec_->isSatisfiedBy(item);
        cache_[item] = result;
        return result;
    }

private:
    mutable std::map<T, bool> cache_;
    std::unique_ptr<Specification<T>> spec_;
};
```

### 3. Support Debug Output

```cpp
template<typename T>
class DebugSpecification : public Specification<T> {
public:
    explicit DebugSpecification(std::unique_ptr<Specification<T>> spec)
        : spec_(std::move(spec)) {}

    bool isSatisfiedBy(const T& item) const override {
        std::cout << "[Debug] Checking: " << item << "\n";
        bool result = spec_->isSatisfiedBy(item);
        std::cout << "[Debug] Result: " << (result ? "PASS" : "FAIL") << "\n";
        return result;
    }

private:
    std::unique_ptr<Specification<T>> spec_;
};
```

## Common Traps

### Trap 1: Overly Complex Specifications

```cpp
// Not recommended: a single specification carries too much logic
class BadSpecification : public Specification<Order> {
public:
    bool isSatisfiedBy(const Order& order) const override {
        // too much logic, hard to test and maintain
        if (order.getTotal() > 100 && order.getCount() > 0
            && order.getCustomer().isVip() && ...) {
            return true;
        }
        return false;
    }
};

// Recommended: split it into several small specifications
class TotalSpecification { ... };
class CountSpecification { ... };
class CustomerSpecification { ... };
```

### Trap 2: Circular Dependencies

```cpp
// Not recommended: specifications depend on each other
class SpecA : public Specification<T> {
    std::shared_ptr<SpecB> specB_;  // depends on SpecB
};

class SpecB : public Specification<T> {
    std::shared_ptr<SpecA> specA_;  // depends on SpecA — circular!
};
```

### Trap 3: Performance Problems

```cpp
// Not recommended: a fresh specification on every validation
bool check(const Order& order) {
    auto spec = std::make_unique<ComplexSpecification>();  // created every time
    return spec->isSatisfiedBy(order);
}

// Recommended: reuse the specification object
class OrderChecker {
public:
    OrderChecker() : spec_(std::make_unique<ComplexSpecification>()) {}

    bool check(const Order& order) {
        return spec_->isSatisfiedBy(order);
    }

private:
    std::unique_ptr<Specification<Order>> spec_;
};
```

## Example

A runnable version of this topic:

- [specification-pattern-demo.cpp](../../examples/specification-pattern-demo.cpp)

## Summary

The key points of the specification pattern:

- **When to use it**: reusable validation logic, complex rules that must be
  combined, business rules that change often
- **Main benefits**: reusable, easy to combine, open-closed friendly
- **Implementation advice**: smart pointers, support combination operations,
  cache results
- **Common traps**: overly complex specifications, circular dependencies,
  performance problems

> **Remember**: the key of the specification pattern is *encapsulating rules* —
> put the validation logic into standalone specification objects so that
> business rules can be reused and combined.
