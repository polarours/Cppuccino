// examples/pitfall-sfinae-misuse.cpp
// Demonstrates that a constraint in the function body is a hard error, while the same constraint in the signature is a soft SFINAE failure.
// Compile with: g++ -std=c++17 -o pitfall_sfinae_misuse pitfall-sfinae-misuse.cpp

#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// --- 1. The broken version ---------------------------------------------------
//
// The requirement is checked in the body:
//
// template <typename T>
// void call_size(const T& value) {
//     value.size();   // HARD error when T has no size()
// }
//
// Calling call_size(42) then produces
//
//   error: request for member 'size' in 'value', which is of non-class type
//   'const int'
//
// and the fallback overload is never considered: a failure inside the body
// is a hard error, not a substitution failure.

void broken_version() {
    std::cout << "--- 1. The broken version ---\n";
    std::cout << "  the requirement 'value.size()' was checked in the body\n";
    std::cout << "  calling it with a type that has no size() is a hard error\n";
    std::cout << "  and the intended fallback overload is never selected\n";
}

// --- 2. The fix: the constraint lives in the signature ------------------------

template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<const T&>().size())>>
    : std::true_type {};

static_assert(has_size<std::vector<int>>::value, "a vector has a size");
static_assert(!has_size<int>::value, "an int has no size");

// The constraint is part of the signature, so a type without size() simply
// removes this overload from the candidate set.
template <typename T>
std::enable_if_t<has_size<T>::value, std::size_t> call_size(const T& value) {
    return value.size();
}

// The fallback is viable only when the overload above was removed.
template <typename T>
std::enable_if_t<!has_size<T>::value, std::size_t> call_size(const T&) {
    return 0;
}

void sfinae_in_action() {
    std::cout << "--- 2. SFINAE in action ---\n";

    std::vector<int> numbers{1, 2, 3};
    std::cout << "  has_size<std::vector<int>>::value = "
              << std::boolalpha << has_size<std::vector<int>>::value << "\n";
    std::cout << "  has_size<int>::value                = " << has_size<int>::value << "\n";

    std::cout << "  call_size(numbers) = " << call_size(numbers) << "\n";
    std::cout << "  call_size(42)      = " << call_size(42)
              << "  (the size-less overload was picked)\n";
    std::cout << "  the second call is not an error: the first overload was\n"
                 "  silently removed from the candidate set\n";
}

// --- 3. Where the immediate context ends -------------------------------------

// The return type is part of the immediate context, so this overload is
// removed softly as well.
template <typename T>
auto call_size_via_return_type(const T& value)
    -> decltype(std::declval<const T&>().size()) {
    return value.size();
}

// A default template argument is part of the immediate context too.
template <typename T, typename = decltype(std::declval<const T&>().size())>
std::size_t call_size_via_default_argument(const T& value) {
    return value.size();
}

void immediate_context() {
    std::cout << "\n--- 3. The immediate context ---\n";

    std::vector<int> numbers{1, 2, 3};
    std::cout << "  call_size_via_return_type(numbers)      = "
              << call_size_via_return_type(numbers) << "\n";
    std::cout << "  call_size_via_default_argument(numbers) = "
              << call_size_via_default_argument(numbers) << "\n";

    // Both calls above are fine, but the same two functions with an int
    // argument are not even considered:
    //
    //   call_size_via_return_type(42);      // no matching function
    //   call_size_via_default_argument(42);  // no matching function
    //
    // That is the difference: "no matching function" is a soft failure, while
    // a missing member in a function body is a hard error.
    std::cout << "  with an int argument both overloads are removed softly,\n"
                 "  while a body check would have been a hard error\n";
}

// --- 4. Do not over-constrain -----------------------------------------------

void over_constraining() {
    std::cout << "\n--- 4. Do not over-constrain ---\n";

    // Two overloads guarded by the same condition are ambiguous:
    //
    // template <typename T>
    // std::enable_if_t<has_size<T>::value, std::size_t> call_size(const T&) { return 1; }
    //
    // template <typename T>
    // std::enable_if_t<has_size<T>::value, std::size_t> call_size(const T&) { return 2; }
    //
    // -> error: call to 'call_size' is ambiguous
    //
    // Keep the conditions mutually exclusive, as call_size() above does.
    std::cout << "  the two call_size overloads above use complementary\n"
                 "  conditions, so exactly one of them is always viable\n";
}

// --- 5. The C++20 spelling --------------------------------------------------

void cxx20_spelling() {
    std::cout << "\n--- 5. The C++20 spelling ---\n";
    std::cout << "  in C++20 the same constraint reads:\n";
    std::cout << "    template <typename T>\n";
    std::cout << "    requires requires(const T& value) { value.size(); }\n";
    std::cout << "    std::size_t call_size(const T& value) { return value.size(); }\n";
    std::cout << "  concepts express the same rule with better diagnostics\n";
}

int main() {
    broken_version();
    sfinae_in_action();
    immediate_context();
    over_constraining();
    cxx20_spelling();

    std::cout << "\n=== All SFINAE scenarios completed ===\n";
    return 0;
}
