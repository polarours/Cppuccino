// examples/structured-bindings.cpp
// Demonstrates structured bindings (C++17) with pairs, tuples, arrays, and structs.
// Compile with: g++ -std=c++17 -o structured_bindings structured-bindings.cpp

#include <iostream>
#include <tuple>
#include <map>
#include <string>
#include <array>
#include <vector>
#include <optional>

// --- 1. Basic Pair ---

void pairBindings() {
    std::cout << "--- Pair Bindings ---\n";

    std::pair<int, std::string> person = {42, "Alice"};

    auto [id, name] = person;
    std::cout << "ID: " << id << ", Name: " << name << "\n";

    // Direct initialization
    auto [x, y] = std::make_pair(3.14, 2.71);
    std::cout << "x: " << x << ", y: " << y << "\n";
}

// --- 2. Tuple ---

void tupleBindings() {
    std::cout << "\n--- Tuple Bindings ---\n";

    std::tuple<int, std::string, double> record = {1, "Bob", 98.5};

    auto [id, name, score] = record;
    std::cout << "ID: " << id << ", Name: " << name << ", Score: " << score << "\n";

    // With std::make_tuple
    auto [a, b, c] = std::make_tuple(10, "hello", 3.14);
    std::cout << "a: " << a << ", b: " << b << ", c: " << c << "\n";
}

// --- 3. Array ---

void arrayBindings() {
    std::cout << "\n--- Array Bindings ---\n";

    std::array<int, 3> arr = {10, 20, 30};
    auto [first, second, third] = arr;

    std::cout << "first: " << first << ", second: " << second << ", third: " << third << "\n";

    // With std::array of different types
    std::array<std::string, 2> names = {"Alice", "Bob"};
    auto [name1, name2] = names;
    std::cout << "name1: " << name1 << ", name2: " << name2 << "\n";
}

// --- 4. Struct ---

struct Point {
    double x;
    double y;
    double z;
};

struct Person {
    std::string name;
    int age;
    double gpa;
};

void structBindings() {
    std::cout << "\n--- Struct Bindings ---\n";

    Point p = {1.0, 2.0, 3.0};
    auto [x, y, z] = p;
    std::cout << "Point: (" << x << ", " << y << ", " << z << ")\n";

    Person person = {"Alice", 20, 3.8};
    auto [name, age, gpa] = person;
    std::cout << "Person: " << name << ", Age: " << age << ", GPA: " << gpa << "\n";
}

// --- 5. Map Iteration ---

void mapIteration() {
    std::cout << "\n--- Map Iteration ---\n";

    std::map<std::string, int> ages = {
        {"Alice", 25},
        {"Bob", 30},
        {"Charlie", 35}
    };

    for (const auto& [name, age] : ages) {
        std::cout << name << ": " << age << "\n";
    }

    // With auto&
    std::map<std::string, std::string> capitals = {
        {"USA", "Washington"},
        {"Japan", "Tokyo"},
        {"France", "Paris"}
    };

    for (auto& [country, capital] : capitals) {
        capital += " (confirmed)";
    }

    for (const auto& [country, capital] : capitals) {
        std::cout << country << ": " << capital << "\n";
    }
}

// --- 6. Vector of Pairs ---

void vectorOfPairs() {
    std::cout << "\n--- Vector of Pairs ---\n";

    std::vector<std::pair<std::string, int>> scores = {
        {"Alice", 95},
        {"Bob", 87},
        {"Charlie", 92}
    };

    for (const auto& [name, score] : scores) {
        std::cout << name << ": " << score << "\n";
    }
}

// --- 7. Function Return Values ---

std::pair<int, std::string> getStatus() {
    return {200, "OK"};
}

std::tuple<double, double, double> getMinMaxAvg(const std::vector<int>& data) {
    if (data.empty()) return {0, 0, 0};

    int min = data[0], max = data[0];
    double sum = 0;

    for (int val : data) {
        if (val < min) min = val;
        if (val > max) max = val;
        sum += val;
    }

    return {static_cast<double>(min), static_cast<double>(max), sum / data.size()};
}

void functionReturnValues() {
    std::cout << "\n--- Function Return Values ---\n";

    auto [code, message] = getStatus();
    std::cout << "Status: " << code << " " << message << "\n";

    std::vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6};
    auto [min, max, avg] = getMinMaxAvg(data);
    std::cout << "Min: " << min << ", Max: " << max << ", Avg: " << avg << "\n";
}

// --- 8. Optional with Structured Bindings ---

std::optional<std::pair<int, int>> findTwoNumbers(const std::vector<int>& nums, int target) {
    for (size_t i = 0; i < nums.size(); ++i) {
        for (size_t j = i + 1; j < nums.size(); ++j) {
            if (nums[i] + nums[j] == target) {
                return std::make_pair(nums[i], nums[j]);
            }
        }
    }
    return std::nullopt;
}

void optionalWithBindings() {
    std::cout << "\n--- Optional with Structured Bindings ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};

    if (auto result = findTwoNumbers(nums, 7)) {
        auto [a, b] = *result;
        std::cout << "Found: " << a << " + " << b << " = 7\n";
    }

    if (auto result = findTwoNumbers(nums, 100)) {
        auto [a, b] = *result;
        std::cout << "Found: " << a << " + " << b << " = 100\n";
    } else {
        std::cout << "No pair found for target 100\n";
    }
}

// --- 9. Nested Structured Bindings ---

void nestedBindings() {
    std::cout << "\n--- Nested Structured Bindings ---\n";

    std::map<std::string, std::pair<int, int>> ranges = {
        {"A", {1, 10}},
        {"B", {11, 20}},
        {"C", {21, 30}}
    };

    for (const auto& [label, range] : ranges) {
        auto [min, max] = range;
        std::cout << label << ": [" << min << ", " << max << "]\n";
    }
}

// --- 10. Ignore Values ---

void ignoreValues() {
    std::cout << "\n--- Ignore Values ---\n";

    auto [id, _] = std::make_pair(42, "ignored");
    std::cout << "ID: " << id << "\n";

    auto [_, name, _2] = std::make_tuple(1, "Alice", 3.14);
    std::cout << "Name: " << name << "\n";
}

int main() {
    pairBindings();
    tupleBindings();
    arrayBindings();
    structBindings();
    mapIteration();
    vectorOfPairs();
    functionReturnValues();
    optionalWithBindings();
    nestedBindings();
    ignoreValues();

    std::cout << "\n=== All structured binding scenarios completed ===\n";
    return 0;
}
