// examples/template_basics.cpp
// Demonstrates function templates, class templates, and template specialization.
// Compile with: g++ -std=c++17 -o template_basics template-basics.cpp

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <type_traits>

// --- 1. Function Templates ---

template <typename T>
T maxOf(T a, T b) {
    return (a > b) ? a : b;
}

// Multiple template parameters
template <typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

// Template with non-type parameter
template <typename T, std::size_t N>
T sumArray(const std::array<T, N>& arr) {
    T total = T{};
    for (const auto& val : arr) {
        total += val;
    }
    return total;
}

void functionTemplateDemo() {
    std::cout << "--- Function Templates ---\n";

    std::cout << "maxOf(3, 7) = " << maxOf(3, 7) << "\n";
    std::cout << "maxOf(3.14, 2.71) = " << maxOf(3.14, 2.71) << "\n";
    std::cout << "maxOf('a', 'z') = " << maxOf('a', 'z') << "\n";

    std::cout << "add(3, 4.5) = " << add(3, 4.5) << "\n";
    std::cout << "add(\"Hello, \", \"World\") = " << add(std::string("Hello, "), std::string("World")) << "\n";

    std::array<int, 4> nums = {1, 2, 3, 4};
    std::cout << "sumArray(nums) = " << sumArray(nums) << "\n";
}

// --- 2. Class Templates ---

template <typename T>
class Stack {
public:
    void push(const T& value) {
        data_.push_back(value);
    }

    T pop() {
        if (data_.empty()) {
            throw std::runtime_error("Stack underflow");
        }
        T value = std::move(data_.back());
        data_.pop_back();
        return value;
    }

    const T& top() const {
        return data_.back();
    }

    bool empty() const {
        return data_.empty();
    }

    std::size_t size() const {
        return data_.size();
    }

private:
    std::vector<T> data_;
};

void classTemplateDemo() {
    std::cout << "\n--- Class Templates ---\n";

    Stack<int> intStack;
    intStack.push(10);
    intStack.push(20);
    intStack.push(30);

    std::cout << "Int stack top: " << intStack.top() << "\n";
    std::cout << "Popped: " << intStack.pop() << "\n";
    std::cout << "Size: " << intStack.size() << "\n";

    Stack<std::string> strStack;
    strStack.push("Hello");
    strStack.push("World");

    std::cout << "String stack top: " << strStack.top() << "\n";
}

// --- 3. Template Specialization ---

template <typename T>
class Printer {
public:
    void print(const T& value) {
        std::cout << "Generic: " << value << "\n";
    }
};

// Full specialization for bool
template <>
class Printer<bool> {
public:
    void print(bool value) {
        std::cout << "Bool: " << (value ? "true" : "false") << "\n";
    }
};

// Partial specialization for pointers
template <typename T>
class Printer<T*> {
public:
    void print(T* value) {
        if (value) {
            std::cout << "Pointer to: " << *value << "\n";
        } else {
            std::cout << "Null pointer\n";
        }
    }
};

void templateSpecializationDemo() {
    std::cout << "\n--- Template Specialization ---\n";

    Printer<int> intPrinter;
    intPrinter.print(42);

    Printer<bool> boolPrinter;
    boolPrinter.print(true);
    boolPrinter.print(false);

    int x = 10;
    Printer<int*> ptrPrinter;
    ptrPrinter.print(&x);
    ptrPrinter.print(nullptr);
}

// --- 4. SFINAE (Substitution Failure Is Not An Error) ---

// Enable function only for integral types
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
safeDivide(T a, T b) {
    if (b == 0) {
        throw std::runtime_error("Division by zero");
    }
    return a / b;
}

// Enable function only for floating point types
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
safeDivide(T a, T b) {
    return a / b;  // IEEE 754 handles division by zero
}

void sfinaeDemo() {
    std::cout << "\n--- SFINAE ---\n";

    std::cout << "safeDivide(10, 3) = " << safeDivide(10, 3) << "\n";
    std::cout << "safeDivide(10.0, 3.0) = " << safeDivide(10.0, 3.0) << "\n";

    try {
        safeDivide(10, 0);
    } catch (const std::exception& e) {
        std::cout << "Caught: " << e.what() << "\n";
    }
}

// --- 5. Variable Templates (C++14) ---

template <typename T>
constexpr T pi = T(3.14159265358979323846);

void variableTemplateDemo() {
    std::cout << "\n--- Variable Templates ---\n";

    std::cout << "pi<int> = " << pi<int> << "\n";
    std::cout << "pi<double> = " << pi<double> << "\n";
    std::cout << "pi<float> = " << pi<float> << "\n";
}

// --- 6. Fold Expressions (C++17) ---

template <typename... Args>
auto sum(Args... args) {
    return (args + ...);
}

template <typename... Args>
void printAll(Args... args) {
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}

void foldExpressionDemo() {
    std::cout << "\n--- Fold Expressions ---\n";

    std::cout << "sum(1, 2, 3, 4, 5) = " << sum(1, 2, 3, 4, 5) << "\n";
    std::cout << "sum(1.1, 2.2, 3.3) = " << sum(1.1, 2.2, 3.3) << "\n";

    std::cout << "printAll: ";
    printAll(1, "hello", 3.14, 'x');
}

// --- 7. Concepts (C++20 preview) ---

// Simulating concepts with SFINAE for C++17
template <typename T>
using EnableIfArithmetic = typename std::enable_if<std::is_arithmetic<T>::value, T>::type;

template <typename T>
EnableIfArithmetic<T> square(T value) {
    return value * value;
}

void conceptPreviewDemo() {
    std::cout << "\n--- Concept-like Constraints (C++17) ---\n";

    std::cout << "square(5) = " << square(5) << "\n";
    std::cout << "square(2.5) = " << square(2.5) << "\n";
    // square("text") would fail: const char* is not arithmetic
}

int main() {
    functionTemplateDemo();
    classTemplateDemo();
    templateSpecializationDemo();
    sfinaeDemo();
    variableTemplateDemo();
    foldExpressionDemo();
    conceptPreviewDemo();

    std::cout << "\n=== All scenarios completed ===\n";
    return 0;
}
