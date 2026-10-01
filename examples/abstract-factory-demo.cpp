// examples/abstract-factory-demo.cpp
// Demonstrates the Abstract Factory Pattern: create families of related
// objects without specifying concrete classes.
// Compile: g++ -std=c++20 -Wall -Wextra -o abstract-factory-demo abstract-factory-demo.cpp

#include <iostream>
#include <memory>
#include <string>

namespace abstract_factory {

// Abstract products
class Button {
public:
    virtual ~Button() = default;
    virtual void paint() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual void paint() const = 0;
};

// Concrete products: Light theme
class LightButton : public Button {
public:
    void paint() const override { std::cout << "LightButton: white background\n"; }
};

class LightCheckbox : public Checkbox {
public:
    void paint() const override { std::cout << "LightCheckbox: white box\n"; }
};

// Concrete products: Dark theme
class DarkButton : public Button {
public:
    void paint() const override { std::cout << "DarkButton: dark background\n"; }
};

class DarkCheckbox : public Checkbox {
public:
    void paint() const override { std::cout << "DarkCheckbox: dark box\n"; }
};

// Abstract factory: guarantees Button+Checkbox belong to the same family
class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual std::unique_ptr<Button> createButton() const = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() const = 0;
};

class LightFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() const override {
        return std::make_unique<LightButton>();
    }
    std::unique_ptr<Checkbox> createCheckbox() const override {
        return std::make_unique<LightCheckbox>();
    }
};

class DarkFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() const override {
        return std::make_unique<DarkButton>();
    }
    std::unique_ptr<Checkbox> createCheckbox() const override {
        return std::make_unique<DarkCheckbox>();
    }
};

// Client code depends only on the abstract factory
void renderUI(const GUIFactory& factory) {
    auto button = factory.createButton();
    auto checkbox = factory.createCheckbox();
    button->paint();
    checkbox->paint();
}

} // namespace abstract_factory

int main() {
    using namespace abstract_factory;
    std::cout << "=== Abstract Factory Demo ===\n\n";

    std::cout << "[Light theme]\n";
    LightFactory light;
    renderUI(light);

    std::cout << "\n[Dark theme]\n";
    DarkFactory dark;
    renderUI(dark);

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
