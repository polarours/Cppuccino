// examples/noexcept_move.cpp
// Demonstrates noexcept and move operations in C++.
// Compile with: g++ -std=c++17 -o noexcept_move noexcept_move.cpp

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

// Class with noexcept move constructor
class NoexceptMove {
public:
    NoexceptMove(const std::string& name) : name_(name), data_(new int[100]) {
        std::cout << "[Acquire] " << name_ << '\n';
    }
    
    ~NoexceptMove() {
        delete[] data_;
        std::cout << "[Release] " << name_ << '\n';
    }
    
    // Move constructor - noexcept
    NoexceptMove(NoexceptMove&& other) noexcept 
        : name_(std::move(other.name_)), data_(other.data_) {
        other.data_ = nullptr;
        std::cout << "[Move] " << name_ << " (noexcept)\n";
    }
    
    // Move assignment - noexcept
    NoexceptMove& operator=(NoexceptMove&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            name_ = std::move(other.name_);
            data_ = other.data_;
            other.data_ = nullptr;
            std::cout << "[Move Assign] " << name_ << " (noexcept)\n";
        }
        return *this;
    }
    
    // Copy operations
    NoexceptMove(const NoexceptMove& other) 
        : name_(other.name_ + "_copy"), data_(new int[100]) {
        std::cout << "[Copy] " << name_ << '\n';
    }
    
    NoexceptMove& operator=(const NoexceptMove& other) {
        if (this != &other) {
            delete[] data_;
            name_ = other.name_ + "_copy";
            data_ = new int[100];
            std::cout << "[Copy Assign] " << name_ << '\n';
        }
        return *this;
    }
    
    std::string name() const { return name_; }

private:
    std::string name_;
    int* data_;
};

// Class with potentially throwing move
class ThrowingMove {
public:
    ThrowingMove(const std::string& name) : name_(name) {}
    
    // Move constructor - might throw
    ThrowingMove(ThrowingMove&& other) : name_(std::move(other.name_)) {
        // Simulate potential failure
        if (name_.empty()) {
            throw std::runtime_error("Move failed");
        }
        std::cout << "[Move] " << name_ << " (might throw)\n";
    }
    
    // Move assignment - might throw
    ThrowingMove& operator=(ThrowingMove&& other) {
        name_ = std::move(other.name_);
        std::cout << "[Move Assign] " << name_ << " (might throw)\n";
        return *this;
    }
    
    std::string name() const { return name_; }

private:
    std::string name_;
};

// Demonstrate vector reallocation behavior
void demonstrateVectorReallocation() {
    std::cout << "=== Vector Reallocation Demo ===\n";
    
    std::vector<NoexceptMove> vec;
    vec.reserve(2);  // Start with capacity 2
    
    std::cout << "\n--- Adding elements (within capacity) ---\n";
    vec.emplace_back("A");
    vec.emplace_back("B");
    
    std::cout << "\n--- Adding element (triggers reallocation) ---\n";
    // Vector will reallocate and move elements
    vec.emplace_back("C");
    
    std::cout << "\n--- Current state ---\n";
    for (const auto& item : vec) {
        std::cout << item.name() << ' ';
    }
    std::cout << '\n';
}

// Demonstrate exception safety with move
void demonstrateExceptionSafety() {
    std::cout << "\n=== Exception Safety Demo ===\n";
    
    std::vector<NoexceptMove> vec;
    vec.reserve(2);
    
    vec.emplace_back("X");
    vec.emplace_back("Y");
    
    try {
        // This might throw, but existing elements are safe
        vec.emplace_back("Z");
    } catch (const std::exception& e) {
        std::cout << "Exception caught: " << e.what() << '\n';
    }
    
    std::cout << "Vector still valid: ";
    for (const auto& item : vec) {
        std::cout << item.name() << ' ';
    }
    std::cout << '\n';
}

// Demonstrate std::move_if_noexcept
void demonstrateMoveIfNoexcept() {
    std::cout << "\n=== std::move_if_noexcept Demo ===\n";
    
    NoexceptMove nm1("NM1");
    ThrowingMove tm1("TM1");
    
    // std::move_if_noexcept returns rvalue only if move is noexcept
    auto&& nm1_moved = std::move_if_noexcept(nm1);
    auto&& tm1_moved = std::move_if_noexcept(tm1);
    
    std::cout << "nm1 after move_if_noexcept: " << nm1.name() << '\n';
    std::cout << "tm1 after move_if_noexcept: " << tm1.name() << '\n';
    
    // Type trait check
    std::cout << "\nNoexceptMove is_nothrow_move_constructible: " 
              << std::is_nothrow_move_constructible_v<NoexceptMove> << '\n';
    std::cout << "ThrowingMove is_nothrow_move_constructible: " 
              << std::is_nothrow_move_constructible_v<ThrowingMove> << '\n';
}

// Demonstrate swap optimization
void demonstrateSwapOptimization() {
    std::cout << "\n=== Swap Optimization Demo ===\n";
    
    NoexceptMove a("Alpha");
    NoexceptMove b("Beta");
    
    std::cout << "Before swap:\n";
    std::cout << "a: " << a.name() << '\n';
    std::cout << "b: " << b.name() << '\n';
    
    // std::swap uses move operations when available and noexcept
    std::swap(a, b);
    
    std::cout << "\nAfter swap:\n";
    std::cout << "a: " << a.name() << '\n';
    std::cout << "b: " << b.name() << '\n';
}

int main() {
    demonstrateVectorReallocation();
    demonstrateExceptionSafety();
    demonstrateMoveIfNoexcept();
    demonstrateSwapOptimization();
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
