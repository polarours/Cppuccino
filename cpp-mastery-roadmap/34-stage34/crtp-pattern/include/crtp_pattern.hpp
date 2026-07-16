#pragma once

#include <string>
#include <iostream>

namespace crtp_pattern {

template <typename Derived>
class Counter {
public:
    static int count() { return count_; }
    static void reset() { count_ = 0; }

protected:
    Counter() { ++count_; }
    Counter(const Counter&) { ++count_; }
    ~Counter() { --count_; }

private:
    static inline int count_ = 0;
};

class Dog : public Counter<Dog> {
public:
    explicit Dog(std::string name) : name_(std::move(name)) {}
    std::string bark() const { return name_ + " says: Woof!"; }
    const std::string& name() const { return name_; }
private:
    std::string name_;
};

class Cat : public Counter<Cat> {
public:
    explicit Cat(std::string name) : name_(std::move(name)) {}
    std::string meow() const { return name_ + " says: Meow!"; }
    const std::string& name() const { return name_; }
private:
    std::string name_;
};

template <typename Derived>
class Printable {
public:
    std::string print() const {
        return static_cast<const Derived*>(this)->toString();
    }
};

class User : public Printable<User> {
public:
    User(std::string name, int age) : name_(std::move(name)), age_(age) {}
    std::string toString() const {
        return "User(" + name_ + ", " + std::to_string(age_) + ")";
    }
private:
    std::string name_;
    int age_;
};

class Product : public Printable<Product> {
public:
    Product(std::string name, double price) : name_(std::move(name)), price_(price) {}
    std::string toString() const {
        return "Product(" + name_ + ", $" + std::to_string(price_) + ")";
    }
private:
    std::string name_;
    double price_;
};

} // namespace crtp_pattern
