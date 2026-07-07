// examples/cpp20-consteval.cpp
// Demonstrates C++20 consteval and constinit.
// Compile with: g++ -std=c++20 -o cpp20_consteval cpp20-consteval.cpp

#include <iostream>
#include <array>
#include <string>

// --- 1. consteval ---

consteval int square(int x) {
    return x * x;
}

consteval int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

void constevalDemo() {
    std::cout << "--- consteval ---\n";

    // Must be evaluated at compile time
    constexpr int sq = square(5);
    std::cout << "square(5) = " << sq << "\n";

    constexpr int fact = factorial(10);
    std::cout << "factorial(10) = " << fact << "\n";

    // Cannot be used at runtime
    // int x = 5;
    // int bad = square(x);  // ERROR: not constant expression
}

// --- 2. constinit ---

constinit int globalInit = 42;
constinit const char* globalStr = "Hello";

void constinitDemo() {
    std::cout << "\n--- constinit ---\n";
    std::cout << "globalInit = " << globalInit << "\n";
    std::cout << "globalStr = " << globalStr << "\n";
}

// --- 3. consteval with constinit ---

consteval int computeAtCompileTime(int a, int b) {
    return a * b + a - b;
}

constinit int computedValue = computeAtCompileTime(10, 5);

void combinedDemo() {
    std::cout << "\n--- consteval + constinit ---\n";
    std::cout << "computedValue = " << computedValue << "\n";
}

// --- 4. constexpr constructor ---

class Point {
public:
    constexpr Point(int x, int y) : x_(x), y_(y) {}

    constexpr int x() const { return x_; }
    constexpr int y() const { return y_; }

    constexpr double distanceTo(const Point& other) const {
        double dx = x_ - other.x_;
        double dy = y_ - other.y_;
        return dx * dx + dy * dy;
    }

private:
    int x_, y_;
};

void constexprConstructorDemo() {
    std::cout << "\n--- constexpr constructor ---\n";

    constexpr Point p1(0, 0);
    constexpr Point p2(3, 4);
    constexpr double dist = p1.distanceTo(p2);
    std::cout << "Distance squared: " << dist << "\n";
}

// --- 5. constexpr if ---

template <typename T>
auto getValue(T value) {
    if constexpr (std::is_integral_v<T>) {
        return value * 2;
    } else if constexpr (std::is_floating_point_v<T>) {
        return value * 3.14;
    } else {
        return value;
    }
}

void constexprIfDemo() {
    std::cout << "\n--- constexpr if ---\n";
    std::cout << "getValue(5) = " << getValue(5) << "\n";
    std::cout << "getValue(3.14) = " << getValue(3.14) << "\n";
}

// --- 6. consteval function with string ---

consteval std::string_view getGreeting() {
    return "Hello, consteval!";
}

void stringConstevalDemo() {
    std::cout << "\n--- consteval string ---\n";
    std::cout << getGreeting() << "\n";
}

// --- 7. consteval lambda ---

consteval auto makeMultiplier(int factor) {
    return [factor](int x) { return x * factor; };
}

void lambdaConstevalDemo() {
    std::cout << "\n--- consteval lambda ---\n";
    constexpr auto doubleIt = makeMultiplier(2);
    constexpr auto tripleIt = makeMultiplier(3);
    std::cout << "doubleIt(5) = " << doubleIt(5) << "\n";
    std::cout << "tripleIt(5) = " << tripleIt(5) << "\n";
}

// --- 8. constexpr array ---

constexpr std::array<int, 10> makeSquareTable() {
    std::array<int, 10> table{};
    for (int i = 0; i < 10; ++i) {
        table[i] = i * i;
    }
    return table;
}

void constexprArrayDemo() {
    std::cout << "\n--- constexpr array ---\n";
    constexpr auto squares = makeSquareTable();
    std::cout << "Squares: ";
    for (int s : squares) std::cout << s << " ";
    std::cout << "\n";
}

// --- 9. consteval vs constexpr ---

consteval int compileTimeOnly(int x) {
    return x + 1;
}

constexpr int bothTimes(int x) {
    return x + 1;
}

void comparisonDemo() {
    std::cout << "\n--- consteval vs constexpr ---\n";

    // Both work at compile time
    constexpr int a = compileTimeOnly(5);
    constexpr int b = bothTimes(5);
    std::cout << "compileTimeOnly(5) = " << a << "\n";
    std::cout << "bothTimes(5) = " << b << "\n";

    // constexpr also works at runtime
    int x = 10;
    int c = bothTimes(x);
    std::cout << "bothTimes(runtime) = " << c << "\n";

    // consteval cannot be used at runtime
    // int d = compileTimeOnly(x);  // ERROR
}

int main() {
    constevalDemo();
    constinitDemo();
    combinedDemo();
    constexprConstructorDemo();
    constexprIfDemo();
    stringConstevalDemo();
    lambdaConstevalDemo();
    constexprArrayDemo();
    comparisonDemo();

    std::cout << "\n=== All consteval scenarios completed ===\n";
    return 0;
}
