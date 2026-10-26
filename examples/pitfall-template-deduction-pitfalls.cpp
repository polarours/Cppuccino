// examples/pitfall-template-deduction-pitfalls.cpp
// Demonstrates forwarding-reference deduction, reference collapsing, non-deduced contexts and CTAD.
// Compile with: g++ -std=c++17 -o pitfall_template_deduction_pitfalls pitfall-template-deduction-pitfalls.cpp

#include <array>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

// --- Helpers that describe a deduced type ------------------------------------

template <typename T>
using lref = std::add_lvalue_reference_t<T>;

template <typename T>
using rref = std::add_rvalue_reference_t<T>;

template <typename T>
std::string describe_type() {
    using Bare = std::remove_cv_t<std::remove_reference_t<T>>;
    std::string base = "unknown";
    if constexpr (std::is_same_v<Bare, int>) {
        base = "int";
    } else if constexpr (std::is_same_v<Bare, std::vector<int>>) {
        base = "vector<int>";
    }
    const std::string prefix = std::is_const_v<std::remove_reference_t<T>> ? "const " : "";
    if constexpr (std::is_lvalue_reference_v<T>) {
        return prefix + base + "&";
    } else {
        return prefix + base;
    }
}

template <typename T>
const char* param_category() {
    return std::is_lvalue_reference_v<decltype(std::declval<T&&>())> ? "lvalue" : "rvalue";
}

// --- 1. Forwarding reference vs plain rvalue reference -----------------------

// In a template, T&& is a forwarding reference: T is deduced from the
// argument and the value category is preserved.
template <typename T>
void forwarding_reference(T&&) {
    std::cout << "  forwarding_reference(T&&):  T = " << describe_type<T>()
              << ", the parameter is an " << param_category<T>() << "\n";
}

// Here T is not a forwarding-reference parameter: the argument must be an
// rvalue and T is always deduced as a value type.
template <typename T>
void rvalue_reference(std::vector<T>&&) {
    std::cout << "  rvalue_reference(vector<T>&&): T = " << describe_type<T>()
              << ", the parameter is an " << param_category<T>() << "\n";
}

void forwarding_references() {
    std::cout << "--- 1. Forwarding reference vs plain rvalue reference ---\n";

    int number = 42;
    const int constant = 7;
    std::vector<int> numbers{1, 2, 3};

    forwarding_reference(number);            // T = int&
    forwarding_reference(42);                // T = int
    forwarding_reference(constant);          // T = const int&
    rvalue_reference(std::vector<int>{1, 2, 3});  // T = int

    // rvalue_reference(numbers);            // ERROR: cannot bind an rvalue
    //                                         // reference to an lvalue
    std::cout << "  a forwarding reference accepts lvalues and rvalues,\n"
                 "  a plain rvalue reference accepts rvalues only\n";
}

// --- 2. Reference collapsing -------------------------------------------------

void reference_collapsing() {
    std::cout << "\n--- 2. Reference collapsing ---\n";

    static_assert(std::is_same_v<lref<lref<int>>, int&>);   // T&  + &  -> T&
    static_assert(std::is_same_v<lref<rref<int>>, int&>);   // T&& + &  -> T&
    static_assert(std::is_same_v<rref<lref<int>>, int&>);   // T&  + && -> T&
    static_assert(std::is_same_v<rref<rref<int>>, int&&>);  // T&& + && -> T&&

    std::cout << "  T&  + &  -> T&\n";
    std::cout << "  T&  + && -> T&\n";
    std::cout << "  T&& + &  -> T&\n";
    std::cout << "  T&& + && -> T&&\n";
    std::cout << "  that is why an lvalue argument makes T a reference\n";
}

// --- 3. Non-deduced contexts -------------------------------------------------

// The iterator type of a container mentions T, but not in a way that allows
// deduction: T is a non-deduced context here.
template <typename T>
void take_iterator(typename std::vector<T>::const_iterator) {
    std::cout << "  take_iterator: T = " << describe_type<T>() << "\n";
}

// An initializer list is a non-deduced context as well.
template <typename T>
void take_vector(std::vector<T>) {
    std::cout << "  take_vector: T = " << describe_type<T>() << "\n";
}

void non_deduced_contexts() {
    std::cout << "\n--- 3. Non-deduced contexts ---\n";

    std::vector<int> numbers{1, 2, 3};

    take_iterator<int>(numbers.begin());  // the argument type must be explicit
    take_vector(numbers);                 // T = int, deduced from the vector
    // take_iterator(numbers.begin());     // ERROR: could not deduce T
    // take_vector({1, 2, 3});             // ERROR: cannot deduce T from an
    //                                     // initializer list
    std::cout << "  T could not be deduced from the iterator type or from\n"
                 "  a braced initializer list, so it has to be spelled out\n";
}

// --- 4. Class template argument deduction ------------------------------------

struct Point {
    double x;
    double y;
};

// An explicit deduction guide (C++17).
auto make_point(double, double) -> Point;

void deduction_guides() {
    std::cout << "\n--- 4. Class template argument deduction ---\n";

    std::pair paren(1, 2.0);        // CTAD -> std::pair<int, double>
    std::pair braces{1, 2.0};      // CTAD -> std::pair<int, double>
    std::array three{1, 2, 3};      // CTAD -> std::array<int, 3>
    Point point{1.0, 2.0};         // the deduction guide makes this work

    static_assert(std::is_same_v<decltype(paren), std::pair<int, double>>);
    static_assert(std::is_same_v<decltype(braces), std::pair<int, double>>);
    static_assert(std::is_same_v<decltype(three), std::array<int, 3>>);
    static_assert(std::is_same_v<decltype(point), Point>);

    std::cout << "  std::pair paren(1, 2.0)   -> std::pair<int, double>\n";
    std::cout << "  std::pair braces{1, 2.0} -> std::pair<int, double>\n";
    std::cout << "  std::array three{1, 2, 3} -> std::array<int, 3>\n";
    std::cout << "  Point point{1.0, 2.0}    -> Point (via the deduction guide)\n";
    std::cout << "  a deduction guide is the documented way to steer CTAD\n";
}

// --- 5. SFINAE only helps in the signature ----------------------------------

// The return type is part of the immediate context, so this overload is
// removed softly when the conversion does not exist.
template <typename T>
auto to_text(const T& value) -> decltype(std::to_string(value)) {
    return std::to_string(value);
}

void sfinae_boundary() {
    std::cout << "\n--- 5. SFINAE only helps in the signature ---\n";

    std::cout << "  to_text(42) = " << to_text(42) << "\n";

    // The same constraint in the body is a hard error:
    //
    // template <typename T>
    // std::string to_text(const T& value) {
    //     return std::to_string(value);   // HARD error for a type without it
    // }
    //
    // to_text(some_complex_number);        // error: no matching function for
    //                                       // call to 'to_string'
    std::cout << "  with a type that has no std::to_string overload the\n"
                 "  call is a soft 'no matching function' failure\n";
}

// --- 6. Prevention -----------------------------------------------------------

void prevention() {
    std::cout << "\n--- 6. Prevention ---\n";
    std::cout << "  * use std::decay_t when a plain value type is meant\n";
    std::cout << "  * spell out template arguments in ambiguous cases\n";
    std::cout << "  * check the deduced type with static_assert\n";
    std::cout << "  * put constraints in the signature, never in the body\n";
}

int main() {
    forwarding_references();
    reference_collapsing();
    non_deduced_contexts();
    deduction_guides();
    sfinae_boundary();
    prevention();

    std::cout << "\n=== All template deduction scenarios completed ===\n";
    return 0;
}
