// examples/template_instantiation.cpp
// Demonstrates template instantiation and two-phase lookup in C++.
// Compile with: g++ -std=c++17 -o template_example template_instantiation.cpp

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <type_traits>

// Basic function template
template<typename T>
T maxOf(T a, T b) {
    return (a > b) ? a : b;
}

// Class template with partial specialization
template<typename T, typename Enable = void>
class Printer {
public:
    void print(const T& value) {
        std::cout << "Generic: " << value << '\n';
    }
};

// Specialization for pointers
template<typename T>
class Printer<T, std::enable_if_t<std::is_pointer_v<T>>> {
public:
    void print(T value) {
        if (value) {
            std::cout << "Pointer to: " << *value << '\n';
        } else {
            std::cout << "Null pointer\n";
        }
    }
};

// Specialization for containers
template<typename T>
class Printer<T, std::enable_if_t<!std::is_pointer_v<T> && 
    requires { typename T::value_type; }>> {
public:
    void print(const T& container) {
        std::cout << "Container [";
        bool first = true;
        for (const auto& item : container) {
            if (!first) std::cout << ", ";
            std::cout << item;
            first = false;
        }
        std::cout << "]\n";
    }
};

// Variadic template
template<typename... Args>
void printAll(Args... args) {
    (std::cout << ... << args) << '\n';
}

// Template with non-type parameter
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

// Two-phase lookup demonstration
template<typename T>
class Container {
public:
    void add(const T& item) {
        items_.push_back(item);
    }
    
    // Dependent name - looked up at instantiation time
    void printAll() const {
        for (const auto& item : items_) {
            std::cout << item << ' ';
        }
        std::cout << '\n';
    }
    
    // Non-dependent name - looked up at definition time
    void clear() {
        items_.clear();
    }

private:
    std::vector<T> items_;
};

int main() {
    std::cout << "=== Function Templates ===\n";
    std::cout << "max(3, 5) = " << maxOf(3, 5) << '\n';
    std::cout << "max(3.14, 2.71) = " << maxOf(3.14, 2.71) << '\n';
    std::cout << "max('a', 'z') = " << maxOf('a', 'z') << '\n';
    
    std::cout << "\n=== Template Specialization ===\n";
    Printer<int> intPrinter;
    intPrinter.print(42);
    
    Printer<int*> ptrPrinter;
    int value = 100;
    ptrPrinter.print(&value);
    ptrPrinter.print(nullptr);
    
    Printer<std::vector<int>> vecPrinter;
    vecPrinter.print(std::vector<int>{1, 2, 3, 4, 5});
    
    Printer<std::string> strPrinter;
    strPrinter.print(std::string("Hello Templates"));
    
    std::cout << "\n=== Variadic Templates ===\n";
    printAll("Hello", " ", "World", "!", " Count: ", 42);
    
    std::cout << "\n=== Compile-time Computation ===\n";
    std::cout << "Factorial<5>::value = " << Factorial<5>::value << '\n';
    std::cout << "Factorial<10>::value = " << Factorial<10>::value << '\n';
    
    std::cout << "\n=== Container Template ===\n";
    Container<std::string> names;
    names.add("Alice");
    names.add("Bob");
    names.add("Charlie");
    names.printAll();
    
    Container<int> numbers;
    for (int i = 1; i <= 5; ++i) {
        numbers.add(i * 10);
    }
    numbers.printAll();
    
    std::cout << "\n=== Type Traits ===\n";
    std::cout << "int is integral: " << std::is_integral_v<int> << '\n';
    std::cout << "double is floating_point: " << std::is_floating_point_v<double> << '\n';
    std::cout << "string is class: " << std::is_class_v<std::string> << '\n';
    
    std::cout << "\n=== End of Program ===\n";
    return 0;
}
