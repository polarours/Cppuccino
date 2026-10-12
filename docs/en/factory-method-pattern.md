# Factory Method Pattern

## What Is the Factory Method Pattern?

The factory method pattern encapsulates "object creation" inside a method,
letting subclasses or registration entries decide which class to instantiate.
Callers depend only on abstract interfaces and never `new` a concrete class
directly.

> **Core idea**: replace `new` with a factory method — move "which class to
> use" from the use site into a registry or subclass.

## When It Fits

- Different implementations must be created from a type string/configuration
- Construction logic is complex (dependency injection, pooling, caching)
- Product types should be extensible at runtime (open-closed principle)

## Core Concepts

- **Product interface**: the common interface of all concrete products
- **Concrete product**: a class implementing Product
- **Factory**: exposes `create(type, ...)` and returns a product per input
- **Registry**: a `type → creator` map replacing `if/else` or `switch`

## Simple Factory vs Factory Method

`if (type=="book") return new Book(...)` is often called a "simple factory" —
but strictly it is not one of the GoF 23; it is a degenerate factory method.
The GoF form pushes creation logic down into **subclasses** or **registered
callbacks**:

```cpp
#include <functional>
#include <map>
#include <memory>
#include <string>

// Degenerate: all the logic lives in one function
struct Product { virtual ~Product() = default; };
struct Book : Product { Book() = default; Book(const std::string&, double) {} };
struct Toy : Product { Toy() = default; Toy(const std::string&, double) {} };

Product* make(std::string type) {
    if (type == "book") return new Book();
    if (type == "toy")  return new Toy();
    return nullptr;
}

// Factory method: one creator per product, registered as callbacks
// (the full ProductFactory appears in the implementation section below)
using Creator = std::function<std::unique_ptr<Product>(const std::string&, double)>;
std::map<std::string, Creator> registry;

void registerCreators() {
    registry["book"] = [](const std::string& n, double p) {
        return std::make_unique<Book>(n, p);
    };
    registry["toy"] = [](const std::string& n, double p) {
        return std::make_unique<Toy>(n, p);
    };
}
```

## Implementation

```cpp
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <map>

class Product {
public:
    virtual ~Product() = default;
    virtual std::string name() const = 0;
    virtual double price() const = 0;
};

class Book : public Product {
public:
    Book(std::string title, double price) : title_(std::move(title)), price_(price) {}
    std::string name() const override { return "Book: " + title_; }
    double price() const override { return price_; }
private:
    std::string title_; double price_;
};

class Electronics : public Product {
public:
    Electronics(std::string model, double price) : model_(std::move(model)), price_(price) {}
    std::string name() const override { return "Electronics: " + model_; }
    double price() const override { return price_; }
private:
    std::string model_; double price_;
};

class ProductFactory {
public:
    using Creator = std::function<std::unique_ptr<Product>(const std::string&, double)>;

    static void registerProduct(const std::string& type, Creator creator) {
        getRegistry()[type] = std::move(creator);
    }

    static std::unique_ptr<Product> create(const std::string& type,
                                           const std::string& name, double price) {
        auto& reg = getRegistry();
        auto it = reg.find(type);
        if (it == reg.end()) throw std::runtime_error("Unknown product: " + type);
        return it->second(name, price);
    }

private:
    static std::map<std::string, Creator>& getRegistry() {
        static std::map<std::string, Creator> r;
        return r;
    }
};
```

For the full runnable example, see the
[factory-pattern project](../../cpp-mastery-roadmap/10-stage10/factory-pattern).

## Usage Example

```cpp
#include <cassert>

ProductFactory::registerProduct("book",
    [](const std::string& n, double p){ return std::make_unique<Book>(n, p); });
ProductFactory::registerProduct("electronics",
    [](const std::string& n, double p){ return std::make_unique<Electronics>(n, p); });

auto p = ProductFactory::create("book", "The C++ Programming Language", 59.99);
assert(p->price() == 59.99);
```

## Best Practices

1. **Register creators with `std::function`**: lighter than inheritance +
   factory subclasses, and lambdas work
2. **Register during translation-unit initialization**: never first-register on
   a concurrent path
3. **Fail loudly on unknown types**: `throw` surfaces calling bugs far earlier
   than returning `nullptr`
4. **Keep product constructor signatures uniform**: all creators take the same
   signature, or callers cannot generalize

## Summary

Factory Method moves "deciding the concrete class" from the caller into a
registry, so a new product costs one `registerProduct` line — the classic way
to eliminate `if/else` chains while staying open for extension.
