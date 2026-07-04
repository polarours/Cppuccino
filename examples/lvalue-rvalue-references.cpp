// examples/lvalue_rvalue_references.cpp
// Demonstrates lvalue and rvalue references in C++.
// Compile with: g++ -std=c++17 -o references lvalue_rvalue_references.cpp

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Function overloading with lvalue/rvalue references
void process(int& x) {
    std::cout << "process(int&): " << x << " (lvalue reference)\n";
    x += 10;
}

void process(const int& x) {
    std::cout << "process(const int&): " << x << " (const lvalue reference)\n";
}

void process(int&& x) {
    std::cout << "process(int&&): " << x << " (rvalue reference)\n";
    x += 100;
}

// Demonstrate value categories
void demonstrateValueCategories() {
    std::cout << "=== Value Categories ===\n";
    
    int a = 42;              // a is lvalue
    const int b = 100;       // b is const lvalue
    int c = a + b;           // (a + b) is rvalue
    
    std::cout << "a (lvalue): " << a << '\n';
    std::cout << "b (const lvalue): " << b << '\n';
    std::cout << "a + b (rvalue): " << c << '\n';
    
    // Function calls
    process(a);              // lvalue -> int&
    process(b);              // const lvalue -> const int&
    process(a + b);          // rvalue -> int&&
    process(42);             // literal -> int&&
}

// Demonstrate reference binding rules
void demonstrateReferenceBinding() {
    std::cout << "\n=== Reference Binding Rules ===\n";
    
    int a = 42;
    const int b = 100;
    
    // Lvalue references
    int& ref1 = a;              // OK: non-const lvalue reference to lvalue
    // int& ref2 = b;           // ERROR: non-const lvalue reference to const lvalue
    const int& ref3 = a;        // OK: const lvalue reference to lvalue
    const int& ref4 = b;        // OK: const lvalue reference to const lvalue
    const int& ref5 = 42;       // OK: const lvalue reference can bind to rvalue
    
    // Rvalue references
    int&& rref1 = 42;           // OK: rvalue reference to rvalue
    // int&& rref2 = a;         // ERROR: rvalue reference cannot bind to lvalue
    const int&& rref3 = a + b;  // OK: const rvalue reference to rvalue
    
    std::cout << "ref1: " << ref1 << '\n';
    std::cout << "ref3: " << ref3 << '\n';
    std::cout << "ref5: " << ref5 << '\n';
    std::cout << "rref1: " << rref1 << '\n';
}

// Demonstrate std::move
void demonstrateStdMove() {
    std::cout << "\n=== std::move Demo ===\n";
    
    std::string s1 = "Hello";
    std::string s2 = "World";
    
    std::cout << "Before move:\n";
    std::cout << "s1: " << s1 << '\n';
    std::cout << "s2: " << s2 << '\n';
    
    // std::move doesn't move anything, it just casts to rvalue reference
    std::string&& rref = std::move(s1);
    std::cout << "\nAfter std::move(s1):\n";
    std::cout << "s1 (still valid): " << s1 << '\n';
    std::cout << "rref: " << rref << '\n';
    
    // Actual move happens here
    std::string s3 = std::move(s2);
    std::cout << "\nAfter s3 = std::move(s2):\n";
    std::cout << "s2 (valid but unspecified): " << s2 << '\n';
    std::cout << "s3: " << s3 << '\n';
}

// Demonstrate move semantics with containers
void demonstrateMoveSemantics() {
    std::cout << "\n=== Move Semantics with Containers ===\n";
    
    std::vector<int> vec1 = {1, 2, 3, 4, 5};
    std::vector<int> vec2;
    
    std::cout << "Before move:\n";
    std::cout << "vec1 size: " << vec1.size() << '\n';
    std::cout << "vec2 size: " << vec2.size() << '\n';
    
    // Move vector content
    vec2 = std::move(vec1);
    
    std::cout << "\nAfter move:\n";
    std::cout << "vec1 size: " << vec1.size() << " (valid but unspecified)\n";
    std::cout << "vec2 size: " << vec2.size() << '\n';
    std::cout << "vec2 contents: ";
    for (int x : vec2) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}

// Demonstrate perfect forwarding setup
template<typename T>
void wrapper(T&& arg) {
    std::cout << "wrapper received: " << arg << '\n';
    // std::forward<T>(arg) would preserve value category
}

void demonstrateForwardingSetup() {
    std::cout << "\n=== Forwarding Setup ===\n";
    
    int a = 42;
    wrapper(a);         // T = int&, arg = int&
    wrapper(42);        // T = int, arg = int&&
    wrapper(std::move(a));  // T = int, arg = int&&
}

int main() {
    demonstrateValueCategories();
    demonstrateReferenceBinding();
    demonstrateStdMove();
    demonstrateMoveSemantics();
    demonstrateForwardingSetup();
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
