# Abstract Factory Pattern

## What Is the Abstract Factory Pattern?

The Abstract Factory Pattern is a creational design pattern that provides an
interface for creating a series of related or dependent objects without
specifying their concrete classes.

> **Core idea**: abstract the factory method pattern one step further — create
> a factory that creates other factories.

## Why Do We Need It?

### Problem Scenario

Suppose we have a game development system that needs to create different kinds
of characters:

```cpp
// Problem: every character type needs its own factory
class CharacterFactory {
public:
    virtual Warrior* createWarrior() = 0;
    virtual Mage* createMage() = 0;
    virtual Healer* createHealer() = 0;
};

// factory for human characters
class HumanFactory : public CharacterFactory {
public:
    Warrior* createWarrior() override { return new HumanWarrior(); }
    Mage* createMage() override { return new HumanMage(); }
    Healer* createHealer() override { return new HumanHealer(); }
};

// factory for orc characters
class OrcFactory : public CharacterFactory {
public:
    Warrior* createWarrior() override { return new OrcWarrior(); }
    Mage* createMage() override { return new OrcMage(); }
    Healer* createHealer() override { return new OrcHealer(); }
};
```

**Problems**:

1. A separate factory has to be written for every character type
2. Adding a new character type means modifying every factory
3. It is hard to create related families of objects

### Solution

Use the abstract factory pattern:

```cpp
// abstract product family
class Warrior {
public:
    virtual ~Warrior() = default;
    virtual std::string attack() = 0;
};

class Mage {
public:
    virtual ~Mage() = default;
    virtual std::string castSpell() = 0;
};

// abstract factory
class CharacterFactory {
public:
    virtual ~CharacterFactory() = default;
    virtual std::unique_ptr<Warrior> createWarrior() = 0;
    virtual std::unique_ptr<Mage> createMage() = 0;
};

// concrete factory
class HumanFactory : public CharacterFactory {
public:
    std::unique_ptr<Warrior> createWarrior() override {
        return std::make_unique<HumanWarrior>();
    }
    std::unique_ptr<Mage> createMage() override {
        return std::make_unique<HumanMage>();
    }
};

class OrcFactory : public CharacterFactory {
public:
    std::unique_ptr<Warrior> createWarrior() override {
        return std::make_unique<OrcWarrior>();
    }
    std::unique_ptr<Mage> createMage() override {
        return std::make_unique<OrcMage>();
    }
};

// client usage
void playGame(CharacterFactory& factory) {
    auto warrior = factory.createWarrior();
    auto mage = factory.createMage();
    
    std::cout << warrior->attack() << "\n";
    std::cout << mage->castSpell() << "\n";
}
```

**Benefits**:

1. Guarantees that related products are created together
2. Makes it easy to switch to another product family
3. Complies with the open-closed principle

## Pattern Structure

```
                    ┌─────────────┐
                    │  Abstract   │
                    │   Factory   │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │HumanFactory │ │OrcFactory   │ │ElfFactory   │ ← concrete factories
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │      AbstractProduct     │ ← abstract product
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │HumanWarrior │ │OrcWarrior   │ │ElfWarrior   │ ← concrete products
    └─────────────┘ └─────────────┘ └─────────────┘
```

## Implementation

### 1. Define the Abstract Products

```cpp
#include <memory>
#include <string>
#include <iostream>

class Button {
public:
    virtual ~Button() = default;
    virtual std::string render() const = 0;
    virtual std::string onClick() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual std::string render() const = 0;
    virtual bool isChecked() const = 0;
};
```

### 2. Implement the Concrete Product Families

```cpp
// Windows-style widgets
class WindowsButton : public Button {
public:
    std::string render() const override {
        return "Rendering Windows button";
    }
    
    std::string onClick() const override {
        return "Windows button clicked";
    }
};

class WindowsCheckbox : public Checkbox {
public:
    std::string render() const override {
        return "Rendering Windows checkbox";
    }
    
    bool isChecked() const override {
        return true;
    }
};

// Mac-style widgets
class MacButton : public Button {
public:
    std::string render() const override {
        return "Rendering Mac button";
    }
    
    std::string onClick() const override {
        return "Mac button clicked";
    }
};

class MacCheckbox : public Checkbox {
public:
    std::string render() const override {
        return "Rendering Mac checkbox";
    }
    
    bool isChecked() const override {
        return false;
    }
};
```

### 3. Define the Abstract Factory

```cpp
class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
};
```

### 4. Implement the Concrete Factories

```cpp
class WindowsFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
    
    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<WindowsCheckbox>();
    }
};

class MacFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }
    
    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<MacCheckbox>();
    }
};
```

### 5. Client Usage

```cpp
class Application {
public:
    explicit Application(std::unique_ptr<GUIFactory> factory)
        : factory_(std::move(factory)) {
        button_ = factory_->createButton();
        checkbox_ = factory_->createCheckbox();
    }
    
    void render() const {
        std::cout << button_->render() << "\n";
        std::cout << checkbox_->render() << "\n";
    }
    
    void handleClick() const {
        std::cout << button_->onClick() << "\n";
    }
    
    bool isChecked() const {
        return checkbox_->isChecked();
    }

private:
    std::unique_ptr<GUIFactory> factory_;
    std::unique_ptr<Button> button_;
    std::unique_ptr<Checkbox> checkbox_;
};

int main() {
    // Windows style
    auto winApp = Application(std::make_unique<WindowsFactory>());
    winApp.render();
    winApp.handleClick();
    
    std::cout << "\n";
    
    // Mac style
    auto macApp = Application(std::make_unique<MacFactory>());
    macApp.render();
    macApp.handleClick();
    
    return 0;
}
```

## Modern C++ Implementations

### With a Factory Registry

```cpp
class FactoryRegistry {
public:
    template<typename T>
    static void registerFactory(const std::string& name) {
        factories_[name] = []() { return std::make_unique<T>(); };
    }
    
    static std::unique_ptr<GUIFactory> create(const std::string& name) {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second();
        }
        throw std::runtime_error("Unknown factory: " + name);
    }

private:
    static std::map<std::string, std::function<std::unique_ptr<GUIFactory>()>> factories_;
};
```

### Simplifying with Templates

```cpp
template<typename ButtonType, typename CheckboxType>
class TypedGUIFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<ButtonType>();
    }
    
    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<CheckboxType>();
    }
};

// usage
using WindowsGUIFactory = TypedGUIFactory<WindowsButton, WindowsCheckbox>;
using MacGUIFactory = TypedGUIFactory<MacButton, MacCheckbox>;
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Factory Method** | The factory method creates a single product; the abstract factory creates a family of products |
| **Builder Pattern** | The builder assembles a complex object step by step; the abstract factory creates a family of related products |
| **Singleton** | The singleton guarantees a single instance; the abstract factory creates families of objects |

## Best Practices

### 1. Use a Factory Method Instead

```cpp
// If you only need to create one product, use a factory method
class SimpleFactory {
public:
    static std::unique_ptr<Button> createButton() {
        return std::make_unique<WindowsButton>();
    }
};

// If you need to create a family of products, use an abstract factory
class ComplexFactory : public GUIFactory {
public:
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
};
```

### 2. Support Product Variants

```cpp
class ThemedButton : public Button {
public:
    explicit ThemedButton(const std::string& theme) : theme_(theme) {}
    
    std::string render() const override {
        return "Rendering themed button (" + theme_ + ")";
    }

private:
    std::string theme_;
};
```

### 3. Use Dependency Injection

```cpp
class DIContainer {
public:
    template<typename T>
    void registerSingleton() {
        instances_[typeid(T).name()] = std::make_shared<T>();
    }
    
    template<typename T>
    std::shared_ptr<T> get() {
        auto it = instances_.find(typeid(T).name());
        if (it != instances_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        throw std::runtime_error("No registration for: " + typeid(T).name());
    }

private:
    std::map<std::string, std::shared_ptr<void>> instances_;
};
```

## Common Traps

### Trap 1: The Factory Is Too Complex

```cpp
// Not recommended: the factory contains too much logic
class BadFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        // too much logic...
        if (condition1) {
            return std::make_unique<WindowsButton>();
        } else if (condition2) {
            return std::make_unique<MacButton>();
        }
        // ...
    }
};

// Recommended: the factory only creates
class GoodFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
};
```

### Trap 2: Violating the Open-Closed Principle

```cpp
// Not recommended: adding a new product requires modifying the factory
class OldFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
    // adding a new product requires changing this
};

// Recommended: extend through inheritance
class NewButtonFactory : public GUIFactory {
    // add the new product here
};
```

### Trap 3: Forgetting to Manage the Lifecycle

```cpp
// Not recommended: manual memory management
class BadApplication {
public:
    BadApplication(GUIFactory* factory) : factory_(factory) {
        button_ = factory_->createButton();  // returns a raw pointer
    }
    
private:
    GUIFactory* factory_;
    Button* button_;  // who is responsible for the delete?
};

// Recommended: use smart pointers
class GoodApplication {
public:
    explicit GoodApplication(std::unique_ptr<GUIFactory> factory)
        : factory_(std::move(factory)) {
        button_ = factory_->createButton();
    }
    
private:
    std::unique_ptr<GUIFactory> factory_;
    std::unique_ptr<Button> button_;
};
```

## Example

A runnable version of this topic:

- [abstract-factory-demo.cpp](../../examples/abstract-factory-demo.cpp)

## Summary

The key points of the abstract factory pattern:

- **When to use it**: related product families have to be created, several
  product variants are needed, or the creation process must be decoupled
- **Main benefits**: guarantees that related products are created together,
  easy to switch product families, complies with the open-closed principle
- **Implementation advice**: use smart pointers, keep the factory simple,
  support dependency injection
- **Common traps**: the factory is too complex, violating the open-closed
  principle, lifecycle management

> **Remember**: the key of the abstract factory pattern is *"product family
> creation"* — create a series of related objects through a single interface
> so that they are guaranteed to work together.
