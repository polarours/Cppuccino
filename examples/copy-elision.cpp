// examples/copy_elision.cpp
// Demonstrates copy elision and prvalue materialization in C++.
// Compile with: g++ -std=c++17 -o copy_elision copy_elision.cpp

#include <iostream>
#include <string>
#include <vector>
#include <utility>

// Class to track copy/move operations
class Tracked {
public:
    Tracked(const std::string& name) : name_(name) {
        std::cout << "[Construct] " << name_ << '\n';
    }
    
    Tracked(const Tracked& other) : name_(other.name_ + "_copy") {
        std::cout << "[Copy] " << name_ << '\n';
    }
    
    Tracked(Tracked&& other) noexcept : name_(std::move(other.name_)) {
        name_ += "_moved";
        std::cout << "[Move] " << name_ << '\n';
    }
    
    ~Tracked() {
        std::cout << "[Destroy] " << name_ << '\n';
    }
    
    Tracked& operator=(const Tracked& other) {
        name_ = other.name_ + "_assigned";
        std::cout << "[Copy Assign] " << name_ << '\n';
        return *this;
    }
    
    Tracked& operator=(Tracked&& other) noexcept {
        name_ = std::move(other.name_);
        name_ += "_move_assigned";
        std::cout << "[Move Assign] " << name_ << '\n';
        return *this;
    }
    
    std::string name() const { return name_; }

private:
    std::string name_;
};

// Named Return Value Optimization (NRVO)
Tracked createNamed() {
    Tracked result("named_result");
    return result;  // NRVO: compiler elides copy/move
}

// Return Value Optimization (RVO) - unnamed
Tracked createUnnamed() {
    return Tracked("unnamed_result");  // RVO: compiler elides copy/move
}

// Demonstrating mandatory copy elision (C++17)
Tracked createTemporary() {
    return Tracked("temporary");  // Mandatory elision in C++17
}

// Function that takes by value (demonstrates pass-by-value elision)
Tracked takeByValue(Tracked input) {
    std::cout << "  Inside function, input: " << input.name() << '\n';
    return input;  // NRVO applied here
}

// Demonstrating perfect forwarding with elision
template<typename T>
T createViaForwarding(T&& arg) {
    return T(std::forward<T>(arg));  // RVO in C++17
}

void demonstrateNRVO() {
    std::cout << "=== Named Return Value Optimization (NRVO) ===\n";
    Tracked t1 = createNamed();
    std::cout << "After assignment: " << t1.name() << '\n';
    std::cout << '\n';
}

void demonstrateRVO() {
    std::cout << "=== Return Value Optimization (RVO) ===\n";
    Tracked t2 = createUnnamed();
    std::cout << "After assignment: " << t2.name() << '\n';
    std::cout << '\n';
}

void demonstrateMandatoryElision() {
    std::cout << "=== Mandatory Copy Elision (C++17) ===\n";
    // In C++17, this is guaranteed to have no copies/moves
    Tracked t3 = createTemporary();
    std::cout << "After assignment: " << t3.name() << '\n';
    std::cout << '\n';
}

void demonstratePassByValue() {
    std::cout << "=== Pass by Value with Elision ===\n";
    Tracked original("original");
    std::cout << "Before function call: " << original.name() << '\n';
    
    Tracked result = takeByValue(std::move(original));
    std::cout << "After function call: " << result.name() << '\n';
    std::cout << '\n';
}

void demonstrateForwardingElision() {
    std::cout << "=== Forwarding with Elision ===\n";
    
    // With lvalue
    Tracked lval("lvalue_example");
    Tracked t1 = createViaForwarding(lval);
    std::cout << "t1: " << t1.name() << '\n';
    
    // With rvalue
    Tracked t2 = createViaForwarding(Tracked("rvalue_example"));
    std::cout << "t2: " << t2.name() << '\n';
    std::cout << '\n';
}

void demonstrateVectorElision() {
    std::cout << "=== Vector and Elision ===\n";
    
    std::vector<Tracked> vec;
    vec.reserve(3);
    
    // emplace_back constructs in-place (no copy/move)
    vec.emplace_back("element1");
    vec.emplace_back("element2");
    vec.emplace_back("element3");
    
    std::cout << "Vector contents:\n";
    for (const auto& item : vec) {
        std::cout << "  " << item.name() << '\n';
    }
    std::cout << '\n';
}

// Prvalue materialization demonstration
void demonstratePrvalueMaterialization() {
    std::cout << "=== Prvalue Materialization ===\n";
    
    // Prvalue (pure rvalue) - temporary object
    int x = 42;        // 42 is prvalue, materialized to bind to x
    double y = 3.14;   // 3.14 is prvalue
    
    // Prvalue in expression
    int z = x + y;     // (x + y) is prvalue, materialized to bind to z
    
    std::cout << "x: " << x << '\n';
    std::cout << "y: " << y << '\n';
    std::cout << "z (x+y): " << z << '\n';
    
    // Temporary materialization conversion
    const int& ref = 42;  // 42 materializes to bind to const reference
    std::cout << "ref: " << ref << '\n';
}

int main() {
    demonstrateNRVO();
    demonstrateRVO();
    demonstrateMandatoryElision();
    demonstratePassByValue();
    demonstrateForwardingElision();
    demonstrateVectorElision();
    demonstratePrvalueMaterialization();
    
    std::cout << "=== End of Program ===\n";
    return 0;
}
