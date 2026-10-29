// examples/null-object-pattern-demo.cpp
// Demonstrates the Null Object Pattern: a null object answers every call with a do-nothing default, so the client never checks for null.
// Compile with: g++ -std=c++17 -o null_object_pattern_demo null-object-pattern-demo.cpp

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

// The null object: every call is a safe no-op with a harmless default.
class NullAnimal : public Animal {
public:
    std::string speak() const override { return "(silence)"; }
    std::string move() const override { return "(nothing)"; }
};

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
