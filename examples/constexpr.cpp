// examples/constexpr.cpp
// Demonstrates constexpr variables, functions, and if constexpr.
// Compile with: g++ -std=c++17 -o constexpr_example constexpr.cpp

#include <iostream>
#include <array>
#include <type_traits>
#include <cmath>

// --- 1. Constexpr Variables ---

constexpr int max_size = 100;
constexpr double pi = 3.14159265358979323846;
constexpr auto greeting = "Hello, constexpr!";

void constexprVariables() {
    std::cout << "--- Constexpr Variables ---\n";
    std::cout << "max_size: " << max_size << "\n";
    std::cout << "pi: " << pi << "\n";
    std::cout << "greeting: " << greeting << "\n";
}

// --- 2. Constexpr Functions ---

constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

constexpr int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}

void constexprFunctions() {
    std::cout << "\n--- Constexpr Functions ---\n";

    // Compile-time evaluation
    constexpr int fact5 = factorial(5);
    constexpr int fib10 = fibonacci(10);
    constexpr int pow2_10 = power(2, 10);

    std::cout << "5! = " << fact5 << "\n";
    std::cout << "fib(10) = " << fib10 << "\n";
    std::cout << "2^10 = " << pow2_10 << "\n";

    // Runtime evaluation (still works)
    int x = 7;
    std::cout << "7! = " << factorial(x) << "\n";
}

// --- 3. Constexpr with Complex Logic ---

constexpr int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

constexpr int abs_val(int x) {
    return (x < 0) ? -x : x;
}

constexpr int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void constexprComplexLogic() {
    std::cout << "\n--- Constexpr with Complex Logic ---\n";

    constexpr int clamped = clamp(15, 0, 10);
    constexpr int abs_neg = abs_val(-42);
    constexpr int g = gcd(48, 18);

    std::cout << "clamp(15, 0, 10) = " << clamped << "\n";
    std::cout << "abs(-42) = " << abs_neg << "\n";
    std::cout << "gcd(48, 18) = " << g << "\n";
}

// --- 4. Constexpr Arrays ---

constexpr std::array<int, 10> makeSquareTable() {
    std::array<int, 10> table{};
    for (int i = 0; i < 10; ++i) {
        table[i] = i * i;
    }
    return table;
}

constexpr std::array<int, 20> makeFibTable() {
    std::array<int, 20> table{};
    table[0] = 0;
    table[1] = 1;
    for (int i = 2; i < 20; ++i) {
        table[i] = table[i-1] + table[i-2];
    }
    return table;
}

void constexprArrays() {
    std::cout << "\n--- Constexpr Arrays ---\n";

    constexpr auto squares = makeSquareTable();
    constexpr auto fibs = makeFibTable();

    std::cout << "Squares: ";
    for (int s : squares) std::cout << s << " ";
    std::cout << "\n";

    std::cout << "Fibonacci: ";
    for (int f : fibs) std::cout << f << " ";
    std::cout << "\n";
}

// --- 5. Constexpr if (C++17) ---

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

template <typename T>
std::string getTypeName() {
    if constexpr (std::is_integral_v<T>) {
        return "integer";
    } else if constexpr (std::is_floating_point_v<T>) {
        return "floating point";
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "string";
    } else {
        return "unknown";
    }
}

void constexprIf() {
    std::cout << "\n--- Constexpr If (C++17) ---\n";

    std::cout << "getValue(5) = " << getValue(5) << "\n";
    std::cout << "getValue(3.14) = " << getValue(3.14) << "\n";
    std::cout << "getValue(\"hello\") = " << getValue("hello") << "\n";

    std::cout << "Type of int: " << getTypeName<int>() << "\n";
    std::cout << "Type of double: " << getTypeName<double>() << "\n";
    std::cout << "Type of string: " << getTypeName<std::string>() << "\n";
}

// --- 6. Constexpr Constructors ---

class Point {
public:
    constexpr Point(double x, double y) : x_(x), y_(y) {}

    constexpr double x() const { return x_; }
    constexpr double y() const { return y_; }

    constexpr double distanceTo(const Point& other) const {
        double dx = x_ - other.x_;
        double dy = y_ - other.y_;
        return std::sqrt(dx * dx + dy * dy);
    }

private:
    double x_, y_;
};

void constexprConstructors() {
    std::cout << "\n--- Constexpr Constructors ---\n";

    constexpr Point p1(0.0, 0.0);
    constexpr Point p2(3.0, 4.0);

    constexpr double dist = p1.distanceTo(p2);
    std::cout << "Distance: " << dist << "\n";

    constexpr Point mid((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);
    std::cout << "Midpoint: (" << mid.x() << ", " << mid.y() << ")\n";
}

// --- 7. Constexpr Lambda ---

void constexprLambda() {
    std::cout << "\n--- Constexpr Lambda ---\n";

    constexpr auto square = [](int x) { return x * x; };
    constexpr auto add = [](int a, int b) { return a + b; };

    constexpr int s = square(5);
    constexpr int sum = add(3, 4);

    std::cout << "square(5) = " << s << "\n";
    std::cout << "add(3, 4) = " << sum << "\n";
}

// --- 8. constexpr vs const ---

void constexprVsConst() {
    std::cout << "\n--- constexpr vs const ---\n";

    const int a = 42;           // Runtime or compile-time
    constexpr int b = 42;      // Compile-time only

    // const can be initialized at runtime
    int runtime_val = 10;
    const int c = runtime_val;  // OK

    // constexpr must be compile-time
    // constexpr int d = runtime_val;  // ERROR

    std::cout << "const a = " << a << "\n";
    std::cout << "constexpr b = " << b << "\n";
    std::cout << "const c = " << c << "\n";
}

int main() {
    constexprVariables();
    constexprFunctions();
    constexprComplexLogic();
    constexprArrays();
    constexprIf();
    constexprConstructors();
    constexprLambda();
    constexprVsConst();

    std::cout << "\n=== All constexpr scenarios completed ===\n";
    return 0;
}
