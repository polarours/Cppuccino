// examples/auto_decltype.cpp
// Demonstrates auto, decltype, and decltype(auto) in C++.
// Compile with: g++ -std=c++17 -o auto_decltype auto_decltype.cpp

#include <iostream>
#include <type_traits>
#include <vector>
#include <string>
#include <utility>

// Function returning different types
int getInt() { return 42; }
double getDouble() { return 3.14; }
const std::string& getString() { 
    static const std::string s = "Hello"; 
    return s; 
}

// Template function using decltype for return type deduction
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

// C++14 return type deduction
auto multiply(int a, int b) {
    return a * b;
}

// Demonstrate auto with different value categories
void demonstrateAuto() {
    std::cout << "=== auto Type Deduction ===\n";
    
    auto x = 42;           // int
    auto y = 3.14;         // double
    auto z = "Hello";      // const char*
    auto s = std::string("World");  // std::string
    
    std::cout << "x: " << x << " (type: " << typeid(x).name() << ")\n";
    std::cout << "y: " << y << " (type: " << typeid(y).name() << ")\n";
    std::cout << "z: " << z << " (type: " << typeid(z).name() << ")\n";
    std::cout << "s: " << s << " (type: " << typeid(s).name() << ")\n";
    
    // auto strips references and cv-qualifiers
    int a = 10;
    int& ref = a;
    const int& cref = a;
    
    auto auto_ref = ref;     // int (not int&)
    auto auto_cref = cref;   // int (not const int&)
    
    std::cout << "\na: " << a << "\n";
    std::cout << "auto_ref: " << auto_ref << "\n";
    std::cout << "auto_cref: " << auto_cref << "\n";
}

// Demonstrate decltype
void demonstrateDecltype() {
    std::cout << "\n=== decltype Type Deduction ===\n";
    
    int a = 42;
    double b = 3.14;
    
    // decltype preserves exact type including references and cv-qualifiers
    decltype(a) da = 0;        // int
    decltype(a + b) dab = 0;   // double (common type)
    decltype((a)) ra = a;      // int& (lvalue expression -> reference)
    
    std::cout << "decltype(a): " << typeid(da).name() << "\n";
    std::cout << "decltype(a+b): " << typeid(dab).name() << "\n";
    std::cout << "decltype((a)): " << typeid(ra).name() << "\n";
    
    // decltype with function calls
    decltype(getInt()) di = getInt();      // int
    decltype(getDouble()) dd = getDouble(); // double
    decltype(getString()) ds = getString(); // const std::string&
    
    std::cout << "\ndecltype(getInt()): " << typeid(di).name() << "\n";
    std::cout << "decltype(getDouble()): " << typeid(dd).name() << "\n";
    std::cout << "decltype(getString()): " << typeid(ds).name() << "\n";
}

// Demonstrate decltype(auto)
template<typename T>
decltype(auto) perfectForward(T&& arg) {
    // Returns exact type of expression, preserving references and cv-qualifiers
    return std::forward<T>(arg);
}

void demonstrateDecltypeAuto() {
    std::cout << "\n=== decltype(auto) Demo ===\n";
    
    int a = 42;
    const int& cref = a;
    
    // decltype(auto) preserves exact type
    decltype(auto) d1 = a;        // int
    decltype(auto) d2 = (a);      // int& (parentheses make it lvalue)
    decltype(auto) d3 = cref;     // const int&
    
    std::cout << "a: " << a << "\n";
    std::cout << "d1 (int): " << d1 << "\n";
    std::cout << "d2 (int&): " << d2 << "\n";
    d2 = 100;  // Modifies a through reference
    std::cout << "After d2 = 100, a: " << a << "\n";
    
    // perfectForward preserves value category
    auto&& forwarded = perfectForward(a);
    std::cout << "forwarded: " << forwarded << "\n";
}

// Type traits with auto and decltype
void demonstrateTypeTraits() {
    std::cout << "\n=== Type Traits with auto/decltype ===\n";
    
    auto x = 42;
    auto y = 3.14;
    auto s = std::string("Hello");
    
    std::cout << "x is int: " << std::is_integral_v<decltype(x)> << "\n";
    std::cout << "y is double: " << std::is_floating_point_v<decltype(y)> << "\n";
    std::cout << "s is class: " << std::is_class_v<decltype(s)> << "\n";
    
    // Check if types are the same
    std::cout << "x and int same: " << std::is_same_v<decltype(x), int> << "\n";
    std::cout << "y and int same: " << std::is_same_v<decltype(y), int> << "\n";
}

// auto with range-based for
void demonstrateAutoWithContainers() {
    std::cout << "\n=== auto with Containers ===\n";
    
    std::vector<int> nums = {1, 2, 3, 4, 5};
    std::vector<std::string> words = {"Hello", "World", "C++"};
    
    // auto with references
    std::cout << "By value:\n";
    for (auto num : nums) {
        std::cout << num << " ";
    }
    std::cout << "\n";
    
    std::cout << "By reference (can modify):\n";
    for (auto& num : nums) {
        num *= 2;
    }
    for (auto num : nums) {
        std::cout << num << " ";
    }
    std::cout << "\n";
    
    std::cout << "By const reference:\n";
    for (const auto& word : words) {
        std::cout << word << " ";
    }
    std::cout << "\n";
}

int main() {
    demonstrateAuto();
    demonstrateDecltype();
    demonstrateDecltypeAuto();
    demonstrateTypeTraits();
    demonstrateAutoWithContainers();
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
