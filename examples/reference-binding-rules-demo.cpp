// examples/reference-binding-rules-demo.cpp
// Demonstrates the rules that decide which reference binds to which expression: the binding matrix, reference collapsing, and the lifetime of a temporary.
// Compile with: g++ -std=c++17 -o reference_binding_rules_demo reference-binding-rules-demo.cpp

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

// --- 1. The binding matrix, checked by the compiler ---

// A reference to a non-const lvalue: it can be read and written.
void takesLvalueRef(int& value) {
    value += 1;
}

// A reference to const: it reads, and it binds to anything that has a
// value, temporary or not.
std::size_t takesConstRef(const std::string& value) {
    return value.size();
}

// A reference to an rvalue: it takes ownership of what it is given.
void takesRvalueRef(std::string&& value) {
    std::cout << "  the rvalue reference took over \"" << value << "\"\n";
}

// The compiler says which bindings are legal. These would not compile:
//
//   takesLvalueRef(42);          // a temporary cannot be written to
//   takesRvalueRef(lvalue);       // an lvalue is not an rvalue
//   takesLvalueRef(constLvalue);  // const does not fit into int&

// --- 2. Reference collapsing ---

// What a reference to a reference really is.
template <typename T>
struct Collapsed;

template <typename T>
struct Collapsed<T&> {
    static constexpr bool isLvalueRef = true;
};

template <typename T>
struct Collapsed<T&&> {
    static constexpr bool isLvalueRef = false;
};

using A = int&;
using B = int&&;

// & & -> &
// & && -> &
// && & -> &
// && && -> &&
static_assert(std::is_same_v<Collapsed<A&>, Collapsed<A>>);
static_assert(std::is_same_v<Collapsed<A&&>, Collapsed<A>>);
static_assert(std::is_same_v<Collapsed<B&>, Collapsed<A>>);
static_assert(std::is_same_v<Collapsed<B&&>, Collapsed<B>>);

// --- 3. The lifetime of a temporary ---

// A temporary bound to a const reference lives as long as the reference.
const int& extended = 42;
static_assert(extended == 42);

// An rvalue reference does not extend anything. The temporary is gone at
// the end of the statement, so reading the reference would be undefined.
// The example does not read it.
std::string&& gone = std::string("a temporary");

int main() {
    std::cout << "=== Reference Binding Rules Demo ===\n\n";

    std::cout << "--- 1. Which reference binds to which expression ---\n";
    int lvalue = 41;
    takesLvalueRef(lvalue);
    std::cout << "  an lvalue binds to int&, and it was written through: "
              << lvalue << "\n";

    const int constLvalue = 7;
    std::cout << "  a const lvalue binds to const int&: "
              << takesConstRef(std::string("hello")) << " characters, and "
              << constLvalue << " is read the same way\n";

    std::cout << "  an rvalue binds to both const int& and int&&\n";
    takesRvalueRef(std::string("moved in"));

    std::cout << "\n--- 2. Reference collapsing ---\n";
    std::cout << "  a reference to a reference is a reference to the\n"
                 "  original type: T& &, T& && and T&& & all become T&,\n"
                 "  and only T&& && stays T&&\n";
    std::cout << "  the compiler checked the four cases while this program\n"
                 "  was being compiled\n";

    std::cout << "\n--- 3. How long a temporary lives ---\n";
    std::cout << "  const int& extended = 42; the temporary lives as long as\n"
                 "  the reference: " << extended << "\n";
    std::cout << "  an rvalue reference extends nothing: the temporary is\n"
                 "  gone at the end of the statement, and reading the\n"
                 "  reference would be undefined\n";

    std::cout << "\n=== All binding rules demonstrated ===\n";
    return 0;
}
