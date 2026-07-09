// examples/std-tuple.cpp
// Demonstrates std::tuple for returning multiple values.
// Compile with: g++ -std=c++17 -o std_tuple std-tuple.cpp

#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// --- 1. Basic usage ---

void basicUsage() {
    std::cout << "--- Basic Usage ---\n";

    std::tuple<int, std::string, double> t(1, "hello", 3.14);

    std::cout << "get<0>: " << std::get<0>(t) << "\n";
    std::cout << "get<1>: " << std::get<1>(t) << "\n";
    std::cout << "get<2>: " << std::get<2>(t) << "\n";
}

// --- 2. Structured bindings ---

void structuredBindings() {
    std::cout << "\n--- Structured Bindings ---\n";

    auto [id, name, score] = std::make_tuple(1, "Alice", 95.5);
    std::cout << "id: " << id << ", name: " << name << ", score: " << score << "\n";
}

// --- 3. Tuple size and element access ---

void tupleSize() {
    std::cout << "\n--- Tuple Size ---\n";

    auto t = std::make_tuple(1, "hello", 3.14, true);
    std::cout << "Size: " << std::tuple_size_v<decltype(t)> << "\n";
}

// --- 4. Make tuple ---

void makeTuple() {
    std::cout << "\n--- Make Tuple ---\n";

    auto t1 = std::make_tuple(1, 2, 3);
    auto t2 = std::make_tuple("hello", 3.14);

    std::cout << "t1: " << std::get<0>(t1) << ", " << std::get<1>(t1) << "\n";
    std::cout << "t2: " << std::get<0>(t2) << ", " << std::get<1>(t2) << "\n";
}

// --- 5. Tie ---

void tieDemo() {
    std::cout << "\n--- Tie ---\n";

    auto t = std::make_tuple(1, "hello", 3.14);
    int id;
    std::string name;
    double score;

    std::tie(id, name, score) = t;
    std::cout << "id: " << id << ", name: " << name << ", score: " << score << "\n";

    // Ignore some values
    std::tie(std::ignore, name, std::ignore) = t;
    std::cout << "name only: " << name << "\n";
}

// --- 6. Tuple comparison ---

void tupleComparison() {
    std::cout << "\n--- Tuple Comparison ---\n";

    auto t1 = std::make_tuple(1, 2, 3);
    auto t2 = std::make_tuple(1, 2, 3);
    auto t3 = std::make_tuple(1, 2, 4);

    std::cout << "t1 == t2: " << (t1 == t2) << "\n";
    std::cout << "t1 < t3: " << (t1 < t3) << "\n";
}

// --- 7. Tuple concatenation ---

void tupleConcatenation() {
    std::cout << "\n--- Tuple Concatenation ---\n";

    auto t1 = std::make_tuple(1, 2);
    auto t2 = std::make_tuple(3, 4, 5);

    auto combined = std::tuple_cat(t1, t2);
    std::cout << "Combined size: " << std::tuple_size_v<decltype(combined)> << "\n";
    std::cout << "Values: ";
    std::apply([](auto&&... args) { ((std::cout << args << " "), ...); }, combined);
    std::cout << "\n";
}

// --- 8. Return multiple values ---

std::tuple<int, int, double> divide(int a, int b) {
    if (b == 0) {
        throw std::runtime_error("Division by zero");
    }
    return {a / b, a % b, static_cast<double>(a) / b};
}

void returnMultipleValues() {
    std::cout << "\n--- Return Multiple Values ---\n";

    auto [quotient, remainder, decimal] = divide(17, 5);
    std::cout << "17 / 5 = " << quotient << " remainder " << remainder << " decimal " << decimal << "\n";
}

// --- 9. Tuple in map ---

void tupleInMap() {
    std::cout << "\n--- Tuple in Map ---\n";

    std::map<std::string, std::tuple<int, std::string>> data;
    data["Alice"] = {25, "Engineer"};
    data["Bob"] = {30, "Designer"};

    for (const auto& [name, info] : data) {
        auto [age, role] = info;
        std::cout << name << ": age " << age << ", role " << role << "\n";
    }
}

// --- 10. Practical example ---

void practicalExample() {
    std::cout << "\n--- Practical Example ---\n";

    // Parse CSV line
    auto parseCSV = [](const std::string& line) -> std::tuple<std::string, int, double> {
        std::string name;
        int age;
        double score;

        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);

        name = line.substr(0, pos1);
        age = std::stoi(line.substr(pos1 + 1, pos2 - pos1 - 1));
        score = std::stod(line.substr(pos2 + 1));

        return {name, age, score};
    };

    auto [name, age, score] = parseCSV("Alice,25,95.5");
    std::cout << "Parsed: " << name << ", age " << age << ", score " << score << "\n";
}

int main() {
    basicUsage();
    structuredBindings();
    tupleSize();
    makeTuple();
    tieDemo();
    tupleComparison();
    tupleConcatenation();
    returnMultipleValues();
    tupleInMap();
    practicalExample();

    std::cout << "\n=== All tuple scenarios completed ===\n";
    return 0;
}
