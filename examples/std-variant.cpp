// examples/std-variant.cpp
// Demonstrates std::variant for type-safe unions.
// Compile with: g++ -std=c++17 -o std_variant std-variant.cpp

#include <iostream>
#include <variant>
#include <string>
#include <vector>
#include <functional>
#include <map>

// --- 1. Basic Usage ---

void basicUsage() {
    std::cout << "--- Basic Usage ---\n";

    std::variant<int, double, std::string> v;

    v = 42;
    std::cout << "int: " << std::get<int>(v) << "\n";

    v = 3.14;
    std::cout << "double: " << std::get<double>(v) << "\n";

    v = "hello";
    std::cout << "string: " << std::get<std::string>(v) << "\n";
}

// --- 2. std::holds_alternative ---

void holdsAlternative() {
    std::cout << "\n--- std::holds_alternative ---\n";

    std::variant<int, double, std::string> v = 42;

    std::cout << "holds int: " << std::holds_alternative<int>(v) << "\n";
    std::cout << "holds double: " << std::holds_alternative<double>(v) << "\n";
    std::cout << "holds string: " << std::holds_alternative<std::string>(v) << "\n";
}

// --- 3. std::visit with visitor ---

void visitDemo() {
    std::cout << "\n--- std::visit ---\n";

    std::variant<int, double, std::string> v = 42;

    std::visit([](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
            std::cout << "int: " << value << "\n";
        } else if constexpr (std::is_same_v<T, double>) {
            std::cout << "double: " << value << "\n";
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "string: " << value << "\n";
        }
    }, v);
}

// --- 4. Overloaded pattern ---

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void overloadedPattern() {
    std::cout << "\n--- Overloaded Pattern ---\n";

    std::variant<int, double, std::string> v = "world";

    std::visit(overloaded{
        [](int i) { std::cout << "int: " << i << "\n"; },
        [](double d) { std::cout << "double: " << d << "\n"; },
        [](const std::string& s) { std::cout << "string: " << s << "\n"; }
    }, v);
}

// --- 5. Variant with custom types ---

struct Circle { double radius; };
struct Rectangle { double width, height; };
struct Triangle { double base, height; };

using Shape = std::variant<Circle, Rectangle, Triangle>;

double area(const Shape& shape) {
    return std::visit([](const auto& s) -> double {
        using T = std::decay_t<decltype(s)>;
        if constexpr (std::is_same_v<T, Circle>) {
            return 3.14159 * s.radius * s.radius;
        } else if constexpr (std::is_same_v<T, Rectangle>) {
            return s.width * s.height;
        } else if constexpr (std::is_same_v<T, Triangle>) {
            return 0.5 * s.base * s.height;
        }
        return 0;
    }, shape);
}

void customTypes() {
    std::cout << "\n--- Custom Types ---\n";

    std::vector<Shape> shapes = {Circle{5.0}, Rectangle{4.0, 6.0}, Triangle{3.0, 8.0}};

    for (const auto& shape : shapes) {
        std::cout << "Area: " << area(shape) << "\n";
    }
}

// --- 6. Variant with std::monostate ---

void monostateDemo() {
    std::cout << "\n--- std::monostate ---\n";

    std::variant<std::monostate, int, std::string> v;

    // Default construction is now possible
    std::cout << "Index: " << v.index() << " (monostate)\n";

    v = 42;
    std::cout << "Index: " << v.index() << " (int)\n";
}

// --- 7. Get with index ---

void getIndexDemo() {
    std::cout << "\n--- Get with Index ---\n";

    std::variant<int, double, std::string> v = 3.14;

    std::cout << "Index: " << v.index() << "\n";
    std::cout << "Value: " << std::get<1>(v) << "\n";
}

// --- 8. Get if ---

void getIfDemo() {
    std::cout << "\n--- std::get_if ---\n";

    std::variant<int, double, std::string> v = 42;

    if (auto* p = std::get_if<int>(&v)) {
        std::cout << "Got int: " << *p << "\n";
    }

    if (auto* p = std::get_if<double>(&v)) {
        std::cout << "Got double: " << *p << "\n";
    } else {
        std::cout << "Not a double\n";
    }
}

// --- 9. Variant in map ---

void variantInMap() {
    std::cout << "\n--- Variant in Map ---\n";

    std::map<std::string, std::variant<int, std::string, double>> config;
    config["timeout"] = 30;
    config["host"] = std::string("localhost");
    config["ratio"] = 0.5;

    for (const auto& [key, value] : config) {
        std::cout << key << ": ";
        std::visit(overloaded{
            [](int v) { std::cout << v; },
            [](double v) { std::cout << v; },
            [](const std::string& v) { std::cout << v; }
        }, value);
        std::cout << "\n";
    }
}

// --- 10. Practical example ---

void practicalExample() {
    std::cout << "\n--- Practical Example ---\n";

    using Value = std::variant<int, double, std::string, bool>;

    auto printValue = [](const Value& v) {
        std::visit(overloaded{
            [](int v) { std::cout << "Integer: " << v; },
            [](double v) { std::cout << "Double: " << v; },
            [](const std::string& v) { std::cout << "String: \"" << v << "\""; },
            [](bool v) { std::cout << "Bool: " << std::boolalpha << v; }
        }, v);
    };

    std::vector<Value> values = {42, 3.14, std::string("hello"), true};

    for (const auto& v : values) {
        printValue(v);
        std::cout << "\n";
    }
}

int main() {
    basicUsage();
    holdsAlternative();
    visitDemo();
    overloadedPattern();
    customTypes();
    monostateDemo();
    getIndexDemo();
    getIfDemo();
    variantInMap();
    practicalExample();

    std::cout << "\n=== All variant scenarios completed ===\n";
    return 0;
}
