# Null Object Pattern

## What Is the Null Object Pattern?

The Null Object Pattern is a behavioral design pattern that gives an interface
a default implementation: an object that does nothing and returns a harmless
value. Instead of handing out a null pointer, your code hands out this null
object, and every call on it is safe.

> **Core idea**: replace "there is no object" with "there is an object that
> does nothing" — the client stops asking whether the object is there.

## Why Do We Need It?

### Problem Scenario

Suppose you have a factory that turns a type name into an animal. A name the
factory does not know has no animal at all, so the factory returns a null
pointer, and every client has to check for it:

```cpp
// Problem: the factory returns null, and every call site has to check
class Animal {
public:
    virtual ~Animal() = default;
    virtual std::string speak() const = 0;
    virtual std::string move() const = 0;
};

class AnimalFactory {
public:
    static std::shared_ptr<Animal> create(const std::string& type) {
        if (type == "dog") return std::make_shared<Dog>();
        if (type == "cat") return std::make_shared<Cat>();
        return nullptr;  // an unknown type: no animal at all
    }
};

// Every client repeats the same check
void report(const std::string& type) {
    auto animal = AnimalFactory::create(type);
    if (!animal) {
        std::cout << "  " << type << ": (no animal)\n";
        return;
    }
    std::cout << "  " << type << ": " << animal->speak() << " - " << animal->move() << "\n";
}
```

**Problems**:

1. Every call site repeats the same check
2. One forgotten check is a crash at run time
3. The "no animal" case is decided in every client instead of in one place

### Solution

Give the interface a null object: a class that answers every call with a
do-nothing default. The factory hands that object out instead of a null
pointer, so the client can use the animal without asking whether it is there:

```cpp
// Solution: the factory never returns null, it returns a null object
class NullAnimal : public Animal {
public:
    std::string speak() const override { return "(silence)"; }
    std::string move() const override { return "(nothing)"; }
};

class AnimalFactory {
public:
    static std::shared_ptr<Animal> create(const std::string& type) {
        if (type == "dog") return std::make_shared<Dog>();
        if (type == "cat") return std::make_shared<Cat>();
        return std::make_shared<NullAnimal>();  // never null
    }
};

// The client no longer checks
void report(const std::string& type) {
    auto animal = AnimalFactory::create(type);
    std::cout << "  " << type << ": " << animal->speak() << " - " << animal->move() << "\n";
}
```

**Benefits**:

1. The client never tests for null
2. The "nothing" case is decided in one place — the factory
3. A new client cannot get the check wrong

The pattern fits when "there is nothing here" is a normal outcome, such as an
empty lookup result. It does not fit when the absence is a failure: a missing
file or a rejected request has to be reported as a failure, not swallowed by a
do-nothing default.

## Pattern Structure

```
                    ┌─────────────┐
                    │   Animal    │
                    │ (interface) │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │    Dog      │ │     Cat      │ │  NullAnimal │ ← null object
    │  (real)     │ │  (real)     │ │   (null)    │
    └─────────────┘ └─────────────┘ └─────────────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
                    ┌──────▼──────┐
                    │    Client   │ ← never checks for null
                    └─────────────┘
```

## Implementation

### 1. Define the Interface

The client codes against this interface, and the interface has a virtual
destructor so that a polymorphic pointer can be deleted safely.

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace null_object {

// The interface the client codes against.
class Animal {
public:
    virtual ~Animal() = default;
    virtual std::string speak() const = 0;
    virtual std::string move() const = 0;
};

} // namespace null_object
```

### 2. Implement the Real Objects

The real objects do their real work; only the null object is allowed to do
nothing.

```cpp
namespace null_object {

// Real animals.
class Dog : public Animal {
public:
    std::string speak() const override { return "Woof!"; }
    std::string move() const override { return "Runs"; }
};

class Cat : public Animal {
public:
    std::string speak() const override { return "Meow!"; }
    std::string move() const override { return "Prowls"; }
};

class Bird : public Animal {
public:
    std::string speak() const override { return "Tweet!"; }
    std::string move() const override { return "Flies"; }
};

} // namespace null_object
```

### 3. Implement the Null Object

Every call on the null object is safe: it returns a harmless default instead
of doing any work.

```cpp
namespace null_object {

// The null object: every call is a safe no-op with a harmless default.
class NullAnimal : public Animal {
public:
    std::string speak() const override { return "(silence)"; }
    std::string move() const override { return "(nothing)"; }
};

} // namespace null_object
```

### 4. Let the Factory Hand Out the Null Object

The factory is the one place that decides what "nothing" means, and it never
returns a null pointer.

```cpp
namespace null_object {

// The factory never hands back a null pointer: an unknown type gives the
// null object instead.
class AnimalFactory {
public:
    static std::shared_ptr<Animal> create(const std::string& type) {
        if (type == "dog") {
            return std::make_shared<Dog>();
        }
        if (type == "cat") {
            return std::make_shared<Cat>();
        }
        if (type == "bird") {
            return std::make_shared<Bird>();
        }
        return std::make_shared<NullAnimal>();
    }
};

} // namespace null_object
```

### 5. Write the Client

The client calls the animal without asking whether it is there. The same
client with a check at every call site still works, but the check is dead
code that the pattern has made unnecessary.

```cpp
// The client: it never asks whether the animal is there.
void show(const null_object::Animal& animal, const std::string& type) {
    std::cout << "  " << type << ": " << animal.speak() << " - " << animal.move() << "\n";
}

int main() {
    std::cout << "=== Null Object Pattern Demo ===\n\n";

    const std::vector<std::string> types = {"dog", "cat", "bird", "fish"};

    std::cout << "-- the client, with no null check --\n";
    for (const std::string& type : types) {
        auto animal = null_object::AnimalFactory::create(type);
        show(*animal, type);
    }

    std::cout << "\n-- the same client, with a null check at every call site --\n";
    for (const std::string& type : types) {
        auto animal = null_object::AnimalFactory::create(type);
        if (animal == nullptr) {  // the check the pattern removes
            std::cout << "  " << type << ": (no animal)\n";
            continue;
        }
        show(*animal, type);
    }

    std::cout << "\n-- the null object is an ordinary object --\n";
    auto unknown = null_object::AnimalFactory::create("fish");
    const bool is_null_animal =
        dynamic_cast<null_object::NullAnimal*>(unknown.get()) != nullptr;
    std::cout << "  an unknown type gives a NullAnimal: "
              << (is_null_animal ? "yes" : "no") << "\n";
    std::cout << "  it still answers every call: "
              << unknown->speak() << " / " << unknown->move() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
```

The project under
[null-object-pattern](../../cpp-mastery-roadmap/13-stage13/null-object-pattern/)
carries the same code in a header, with a test suite on top of it.

## Modern C++ Implementations

### One Shared Instance

The null object holds no state, so one instance can serve every client and
the factory does not allocate a new one for each lookup.

```cpp
// The null object holds no state, so a single instance serves everyone.
std::shared_ptr<null_object::Animal> null_animal() {
    static const std::shared_ptr<null_object::Animal> shared =
        std::make_shared<null_object::NullAnimal>();
    return shared;
}
```

### A Null Object with Configurable Defaults

A null object does not have to be a fixed set of strings: it can take the
defaults it returns as constructor arguments.

```cpp
class ConfigurableNullAnimal : public null_object::Animal {
public:
    ConfigurableNullAnimal(std::string speak_text, std::string move_text)
        : speak_(std::move(speak_text)), move_(std::move(move_text)) {}

    std::string speak() const override { return speak_; }
    std::string move() const override { return move_; }

private:
    std::string speak_;
    std::string move_;
};
```

### A Null Object as a Default Member Value

You do not need a factory for this. A member that is a null object is never
null, so the code that uses it never checks it.

```cpp
class NullLogger {
public:
    void log(const std::string&) const {}  // the do-nothing default
};

class Report {
public:
    void print() { logger_.log("report printed"); }

private:
    NullLogger logger_;  // never null, so nobody has to check it
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Proxy** | A proxy forwards every call to a real object; a null object has no real object behind it |
| **`std::optional`** | An optional moves the empty case into the caller, who has to handle it; a null object keeps the default inside the object |
| **Singleton** | A singleton shares one instance of a real class; a null object shares one instance of a do-nothing class |
| **Strategy** | A strategy changes what the client does; a null object changes only what happens when there is nothing to work with |

## Best Practices

### 1. Return the Null Object from One Place

The pattern holds only as long as every exit of the factory hands back a
usable object.

```cpp
// Recommended: the factory is the only place that decides
std::shared_ptr<Animal> create(const std::string& type) {
    if (type == "dog") return std::make_shared<Dog>();
    if (type == "cat") return std::make_shared<Cat>();
    return std::make_shared<NullAnimal>();  // the one null-free exit
}

// Not recommended: a second path that hands out a raw null
std::shared_ptr<Animal> create_unchecked(const std::string& type) {
    if (type == "dog") return std::make_shared<Dog>();
    return nullptr;  // the pattern breaks here
}
```

### 2. Keep the Defaults Honest

The default should say what happened instead of pretending that something
happened.

```cpp
// Recommended: the default says what happened
class NullAnimal : public Animal {
public:
    std::string speak() const override { return "(silence)"; }
    std::string move() const override { return "(nothing)"; }
};

// Not recommended: the default pretends the animal was there
class LyingNullAnimal : public Animal {
public:
    std::string speak() const override { return "Woof!"; }
    std::string move() const override { return "Runs"; }
};
```

### 3. Reserve the Null Object for Absence

The null object means "there is nothing here". When the absence is a failure,
report the failure instead, or it disappears into a silent default.

```cpp
// Recommended: the null object stands for an ordinary empty result
auto animal = AnimalFactory::create("fish");
std::cout << animal->speak();  // "(silence)"

// Not recommended: a failure is turned into an ordinary empty result
auto report = load_report("missing.txt");
report->render();  // an empty report instead of a loud failure
```

## Common Traps

### Trap 1: The Null Object Swallows a Real Failure

If every kind of failure returns the null object, the client cannot tell an
unknown type from a broken system.

```cpp
// Not recommended: a broken system looks exactly like an unknown type
std::shared_ptr<Animal> create_or_null(const std::string& type) {
    if (type == "dog") return std::make_shared<Dog>();
    if (!registry_ready()) {
        return std::make_shared<NullAnimal>();  // the failure is hidden
    }
    return std::make_shared<NullAnimal>();
}

// Recommended: the failure is reported, the null object only means "nothing"
std::shared_ptr<Animal> create_or_fail(const std::string& type) {
    if (!registry_ready()) {
        throw std::runtime_error("the animal registry is not ready");
    }
    if (type == "dog") return std::make_shared<Dog>();
    return std::make_shared<NullAnimal>();
}
```

### Trap 2: The Null Object Returns a Value That Lies

A default that claims the work was done hides the very problem the null
object was meant to make visible.

```cpp
class Writer {
public:
    virtual ~Writer() = default;
    virtual bool write(const std::string& text) = 0;
};

// Not recommended: the default pretends the write succeeded
class LyingNullWriter : public Writer {
public:
    bool write(const std::string&) override { return true; }
};

// Recommended: the default says that nothing was written
class NullWriter : public Writer {
public:
    bool write(const std::string&) override { return false; }
};
```

### Trap 3: Slicing the Null Object

The null object is a derived class like any other, so handing it out by value
cuts it down to the base class and breaks it.

```cpp
// Not recommended: returning the null object by value slices it
Animal sliced_null_animal() {
    return NullAnimal();  // only the Animal part survives
}

// Recommended: hand it out through a pointer, like every other animal
std::shared_ptr<Animal> null_animal_ptr() {
    return std::make_shared<NullAnimal>();
}
```

## Example

A runnable version of this topic:

- [null-object-pattern-demo.cpp](../../examples/null-object-pattern-demo.cpp)

## Summary

The key points of the null object pattern:

- **When to use it**: a lookup may legitimately find nothing, and the client
  code would otherwise repeat the same null check everywhere
- **Main benefits**: the client never tests for null, one place decides what
  "nothing" means, a new client cannot get the check wrong
- **Implementation advice**: let the factory hand out the null object, keep
  the defaults honest, keep the null object stateless
- **Common traps**: swallowing a real failure, a default that lies, slicing
  the null object
- **Related docs**: [Strategy Pattern](strategy-pattern.md),
  [Factory Method Pattern](factory-method-pattern.md),
  [Proxy Pattern](proxy-pattern.md)
- **Pitfalls**: [Dangling Pointer](../../pitfalls/en/dangling-pointer.md),
  [Object Slicing](../../pitfalls/en/object-slicing.md),
  [Memory Leak](../../pitfalls/en/memory-leak.md)

> **Remember**: the key of the null object pattern is *a default that answers
> every call* — give the interface an object that does nothing, and the client
> never has to ask whether the object is there.
