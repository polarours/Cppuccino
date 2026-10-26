// examples/pitfall-template-specialization-pitfalls.cpp
// Demonstrates template specialization mistakes: partial specialization of functions, std templates, and silent misses.
// Compile with: g++ -std=c++17 -o pitfall_template_specialization_pitfalls pitfall-template-specialization-pitfalls.cpp

#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

// --- 1. A function template cannot be partially specialized ---

template <typename T>
void print(const T& value) {
    std::cout << "value: " << value << "\n";
}

// BAD: this is a partial specialization of a function template, which the
// language forbids. The compiler rejects it with "function template partial
// specialization is not allowed", so it is kept commented out.
// template <typename T>
// void print<T*>(const T* value) { std::cout << "ptr: " << *value << "\n"; }

// GOOD: an overload with the same effect. For pointer arguments the pointer
// overload is the more specialized one, so partial ordering selects it.
// (Note that the pointer parameter has to be T* and not const T*, otherwise
// neither overload is more specialized and the call stays ambiguous.)
template <typename T>
void print(T* value) {
    std::cout << "ptr:  " << *value << "\n";
}

// --- 2. A specialization that is declared too late is not used ---

template <typename T>
struct Describe {
    static std::string text() { return "primary template"; }
};

template <>
struct Describe<int> {
    static std::string text() { return "specialization for int"; }
};

// BAD: using Describe<int> before the specialization above is declared makes
// the primary template the point of instantiation. The specialization then
// comes too late: GCC rejects it with "specialization of 'Describe<int>' after
// instantiation", while other compilers are allowed to ignore it silently.
// The standard only says that no diagnostic is required.
// void useBeforeTheSpecializationIsDeclared() {
//     std::cout << Describe<int>::text() << "\n";
// }

// --- 3. A partial specialization that does not match is silently skipped ---

template <typename T>
struct Traits {
    static std::string name() { return "generic"; }
};

// The pattern only matches "pointer to const", so Traits<int*> keeps using
// the primary template. The program compiles and quietly gives the wrong
// answer: no diagnostic points at the mismatch.
template <typename T>
struct Traits<const T*> {
    static std::string name() { return "pointer to const"; }
};

// --- 4. A specialization has to live in the same namespace ---

namespace tools {

template <typename T>
struct Wrapper {
    static std::string name() { return "generic wrapper"; }
};

}  // namespace tools

namespace tools {

// GOOD: the specialization is in the same namespace as the primary template.
template <>
struct Wrapper<int> {
    static std::string name() { return "int wrapper"; }
};

}  // namespace tools

struct MyType {
    int x{0};
};

// BAD: a specialization in the wrong namespace is not a specialization of
// tools::Wrapper at all, so the compiler reports "specialization of
// 'tools::Wrapper<int>' after instantiation" or simply ignores it.
// namespace other {
// template <>
// struct tools::Wrapper<int> { static std::string name() { return "wrong"; } };
// }

}  // namespace

// --- 5. Specializing a std template for a user defined type is allowed ---

struct Point {
    int x{0};
    int y{0};

    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

namespace std {

template <>
struct hash<Point> {
    std::size_t operator()(const Point& point) const noexcept {
        const std::size_t hx = std::hash<int>{}(point.x);
        const std::size_t hy = std::hash<int>{}(point.y);
        return hx ^ (hy + 0x9e3779b9u + (hx << 6) + (hx >> 2));
    }
};

}  // namespace std

int main() {
    std::cout << "--- Function templates: overload instead of partial specialization ---\n";
    int number = 7;
    print(42);
    print(&number);

    std::cout << "\n--- A declared specialization is used ---\n";
    std::cout << "Describe<int>::text() = " << Describe<int>::text() << "\n";

    std::cout << "\n--- A partial specialization that does not match is skipped ---\n";
    std::cout << "Traits<int>::name()     -> " << Traits<int>::name() << "\n";
    std::cout << "Traits<int*>::name()    -> " << Traits<int*>::name()
              << " (the partial specialization only matches const T*)\n";
    std::cout << "Traits<const int*>::name() -> " << Traits<const int*>::name() << "\n";

    std::cout << "\n--- Specializations belong to the primary template's namespace ---\n";
    std::cout << "tools::Wrapper<int> -> " << tools::Wrapper<int>::name() << "\n";

    std::cout << "\n--- Customizing a std template for a user defined type ---\n";
    // BAD: specializing a standard container is undefined behaviour, so it
    // is kept commented out.
    // namespace std {
    // template <> class vector<MyType> { /* ... */ };
    // }
    // GOOD: use an alias (or your own type) instead.
    using MyVector = std::vector<MyType>;
    MyVector values{{1}, {2}, {3}};
    std::cout << "alias vector size: " << values.size() << "\n";

    std::unordered_map<Point, int> counts;
    counts[Point{1, 2}] = 3;
    counts[Point{1, 2}] += 4;
    std::cout << "hash specialization works, count = " << counts[Point{1, 2}] << "\n";

    std::cout << "\n=== All template specialization scenarios completed ===\n";
    return 0;
}
