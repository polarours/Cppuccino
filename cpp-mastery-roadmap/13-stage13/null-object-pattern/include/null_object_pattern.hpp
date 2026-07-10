#pragma once

#include <string>
#include <iostream>
#include <memory>

namespace null_object_pattern {

// --- Interface ---

class Animal {
public:
    virtual ~Animal() = default;
    virtual std::string speak() const = 0;
    virtual std::string move() const = 0;
};

// --- Concrete Animals ---

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

// --- Null Object ---

class NullAnimal : public Animal {
public:
    std::string speak() const override { return "(silence)"; }
    std::string move() const override { return "(nothing)"; }
};

// --- Factory ---

class AnimalFactory {
public:
    static std::shared_ptr<Animal> create(const std::string& type) {
        if (type == "dog") return std::make_shared<Dog>();
        if (type == "cat") return std::make_shared<Cat>();
        if (type == "bird") return std::make_shared<Bird>();
        return std::make_shared<NullAnimal>();
    }
};

} // namespace null_object_pattern
