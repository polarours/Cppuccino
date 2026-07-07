// examples/cpp20-concepts.cpp
// Demonstrates C++20 concepts for template constraints.
// Compile with: g++ -std=c++20 -o cpp20_concepts cpp20-concepts.cpp

#include <iostream>
#include <concepts>
#include <string>
#include <vector>
#include <list>
#include <algorithm>

// --- 1. Basic Concept ---

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T>
T add(T a, T b) {
    return a + b;
}

void basicConcept() {
    std::cout << "--- Basic Concept ---\n";
    std::cout << "add(3, 4) = " << add(3, 4) << "\n";
    std::cout << "add(1.5, 2.5) = " << add(1.5, 2.5) << "\n";
}

// --- 2. Requires Expression ---

template <typename T>
concept Container = requires(T t) {
    typename T::value_type;
    typename T::iterator;
    { t.begin() } -> std::same_as<typename T::iterator>;
    { t.end() } -> std::same_as<typename T::iterator>;
    { t.size() } -> std::convertible_to<std::size_t>;
};

template <Container C>
void printContainer(const C& c) {
    std::cout << "[";
    bool first = true;
    for (const auto& val : c) {
        if (!first) std::cout << ", ";
        std::cout << val;
        first = false;
    }
    std::cout << "]\n";
}

void requiresExpression() {
    std::cout << "\n--- Requires Expression ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::list<std::string> lst = {"a", "b", "c"};

    std::cout << "vector: ";
    printContainer(vec);

    std::cout << "list: ";
    printContainer(lst);
}

// --- 3. Concept with Requirements ---

template <typename T>
concept Sortable = requires(T t) {
    { t.begin() } -> std::input_or_output_iterator;
    { t.end() } -> std::input_or_output_iterator;
    requires std::totally_ordered<typename T::value_type>;
};

template <Sortable C>
void sortAndPrint(C& c) {
    std::sort(c.begin(), c.end());
    printContainer(c);
}

void conceptWithRequirements() {
    std::cout << "\n--- Concept with Requirements ---\n";

    std::vector<int> vec = {5, 3, 1, 4, 2};
    std::cout << "Before sort: ";
    printContainer(vec);

    sortAndPrint(vec);
}

// --- 4. Compound Requirements ---

template <typename T>
concept Hashable = requires(T t) {
    { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

template <Hashable T>
void hashAndPrint(const T& value) {
    std::size_t hash = std::hash<T>{}(value);
    std::cout << "hash(" << value << ") = " << hash << "\n";
}

void compoundRequirements() {
    std::cout << "\n--- Compound Requirements ---\n";
    hashAndPrint(42);
    hashAndPrint(std::string("hello"));
}

// --- 5. Nested Requirements ---

template <typename T>
concept NumericContainer = Container<T> && requires {
    typename T::value_type;
    requires Numeric<typename T::value_type>;
};

template <NumericContainer C>
auto sum(const C& c) {
    typename C::value_type total{};
    for (const auto& val : c) {
        total += val;
    }
    return total;
}

void nestedRequirements() {
    std::cout << "\n--- Nested Requirements ---\n";

    std::vector<int> nums = {1, 2, 3, 4, 5};
    std::cout << "sum(vector) = " << sum(nums) << "\n";

    std::vector<double> doubles = {1.5, 2.5, 3.0};
    std::cout << "sum(doubles) = " << sum(doubles) << "\n";
}

// --- 6. Constrained Auto ---

void constrainedAuto() {
    std::cout << "\n--- Constrained Auto ---\n";

    auto value = 42;  // int
    auto pi = 3.14;   // double

    // Constrained auto in C++20
    // (Would need concepts for full constraining)
    std::cout << "value: " << value << "\n";
    std::cout << "pi: " << pi << "\n";
}

// --- 7. Requires Clauses ---

template <typename T>
    requires std::is_trivially_copyable_v<T>
T safeCopy(T value) {
    return value;
}

void requiresClauses() {
    std::cout << "\n--- Requires Clauses ---\n";

    int x = safeCopy(42);
    double d = safeCopy(3.14);
    std::cout << "safeCopy(42) = " << x << "\n";
    std::cout << "safeCopy(3.14) = " << d << "\n";
}

// --- 8. Concept-Based Overloading ---

template <typename T>
void process(T value) requires std::integral<T> {
    std::cout << "Integral: " << value << "\n";
}

template <typename T>
void process(T value) requires std::floating_point<T> {
    std::cout << "Floating: " << value << "\n";
}

void conceptOverloading() {
    std::cout << "\n--- Concept-Based Overloading ---\n";
    process(42);
    process(3.14);
}

// --- 9. Custom Concept with Member Requirements ---

template <typename T>
concept Printable = requires(std::ostream& os, const T& t) {
    { os << t } -> std::same_as<std::ostream&>;
};

template <Printable T>
void print(const T& value) {
    std::cout << value << "\n";
}

struct Point {
    int x, y;
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        return os << "(" << p.x << ", " << p.y << ")";
    }
};

void customConcept() {
    std::cout << "\n--- Custom Concept ---\n";
    print(42);
    print("hello");
    print(Point{1, 2});
}

// --- 10. Concept for Range ---

template <typename R>
concept Range = requires(R& r) {
    std::begin(r);
    std::end(r);
};

template <Range R>
void printRange(const R& range) {
    std::cout << "[";
    bool first = true;
    for (const auto& val : range) {
        if (!first) std::cout << ", ";
        std::cout << val;
        first = false;
    }
    std::cout << "]\n";
}

void rangeConcept() {
    std::cout << "\n--- Range Concept ---\n";

    std::vector<int> vec = {1, 2, 3};
    int arr[] = {4, 5, 6};

    printRange(vec);
    printRange(arr);
}

int main() {
    basicConcept();
    requiresExpression();
    conceptWithRequirements();
    compoundRequirements();
    nestedRequirements();
    constrainedAuto();
    requiresClauses();
    conceptOverloading();
    customConcept();
    rangeConcept();

    std::cout << "\n=== All concepts scenarios completed ===\n";
    return 0;
}
