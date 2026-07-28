#pragma once
#include <string>
#include <iostream>
#include <utility> // for std::swap

class DemoObject {
public:
    // Constructor with label for identification
    explicit DemoObject(const std::string& id) : id_(id) {
        std::cout << "[CONSTRUCT] " << id_ << "\n";
    }

    // Copy constructor (called when copying an lvalue)
    DemoObject(const DemoObject& other) : id_(other.id_ + "_copy") {
        std::cout << "[COPY CTOR]  " << id_ << " (from " << other.id_ << ")\n";
    }

    // Move constructor (called when moving an rvalue)
    DemoObject(DemoObject&& other) noexcept : id_(other.id_ + "_move") {
        std::cout << "[MOVE CTOR]  " << id_ << " (from " << other.id_ << ")\n";
    }

    // Copy assignment operator
    DemoObject& operator=(const DemoObject& other) {
        if (this != &other) {
            std::cout << "[COPY ASSGN] " << id_ << " (to " << other.id_ << ")\n";
            id_ = other.id_ + "_copy";
        }
        return *this;
    }

    // Move assignment operator
    DemoObject& operator=(DemoObject&& other) noexcept {
        if (this != &other) {
            std::cout << "[MOVE ASSGN] " << id_ << " (to " << other.id_ << ")\n";
            id_ = other.id_ + "_move";
        }
        return *this;
    }

    // Destructor
    ~DemoObject() {
        std::cout << "[DESTRUCT]   " << id_ << "\n";
    }

    // Get the object's ID
    const std::string& getId() const { return id_; }

    // Print function (demonstrates use after copy/move)
    void print() const {
        std::cout << id_ << "\n";
    }

private:
    std::string id_;

    // Friend declaration for swap (can access private members)
    friend void swap(DemoObject&, DemoObject&) noexcept;
};

// Non-member swap function (required for move semantics in containers)
inline void swap(DemoObject& a, DemoObject& b) noexcept {
    using std::swap;
    swap(a.id_, b.id_);
}
