#include <iostream>
#include <string>
#include <utility>  // for std::move, std::forward

// Forward declarations for function parameter tests
void f1(int& ref);
void f2(const int& ref);
void f3(int&& ref);

// Template function for perfect forwarding demonstration (must be outside functions)
template<typename T>
void wrapper(T&& arg) {
    // arg is a forwarding reference - can bind to both lvalues and rvalues
    // Inside the function, arg is an lvalue (need std::move to forward)
    // another_function(std::forward<T>(arg));
}

// Define the helper functions
void f1(int& ref) { ref = 10; }
void f2(const int& ref) { /* read-only access */ }
void f3(int&& ref) { ref = 20; }

void test_lvalue_ref_binding() {
    std::cout << "=== Lvalue Reference Binding ===\n";

    int x = 42;  // x is an lvalue

    // lvalue reference to non-const can bind to lvalue
    int& r1 = x;
    std::cout << "int& r1 = x;  // OK: lvalue ref to non-const binds to lvalue\n";

    // const lvalue reference can bind to lvalue
    const int& r2 = x;
    std::cout << "const int& r2 = x;  // OK: const lvalue ref binds to lvalue\n";

    // lvalue reference to non-const cannot bind to rvalue (error, commented out)
    // int& r3 = 42;  // ERROR: cannot bind non-const lvalue ref to rvalue

    // rvalue reference cannot bind to lvalue (error, commented out)
    // int&& r4 = x;  // ERROR: rvalue ref cannot bind to lvalue

    std::cout << "\n";
}

void test_rvalue_ref_binding() {
    std::cout << "=== Rvalue Reference Binding ===\n";

    int x = 42;

    // rvalue reference binds to rvalue
    int&& r1 = 42;
    std::cout << "int&& r1 = 42;  // OK: rvalue ref binds to rvalue (temporary)\n";

    // To bind rvalue ref to an lvalue, need std::move
    int&& r2 = std::move(x);
    std::cout << "int&& r2 = std::move(x);  // OK: explicitly cast lvalue to rvalue\n";

    // After move, x is in valid but unspecified state

    std::cout << "\n";
}

void test_function_params() {
    std::cout << "=== Function Parameter Binding ===\n";

    int a = 5;
    f1(a);  // OK
    std::cout << "void f1(int&); f1(a);  // OK: lvalue binds to non-const lvalue ref\n";

    int b = 5;
    f2(b);      // OK: lvalue binds to const lvalue ref
    f2(42);     // OK: rvalue binds to const lvalue ref (extends lifetime)
    std::cout << "void f2(const int&); f2(lval); f2(rval);  // Both OK\n";

    f3(std::move(b));  // OK: explicit move
    f3(42);    // OK: rvalue binds to rvalue ref
    std::cout << "void f3(int&&); f3(std::move(lval)); f3(rval);  // OK\n";

    std::cout << "\n";
}

void test_reference_collapsing() {
    std::cout << "=== Reference Collapsing (Perfect Forwarding) ===\n";

    int x = 42;
    wrapper(x);          // T = int&, arg becomes int&& -> collapses to int&
    wrapper(std::move(x)); // T = int, arg becomes int&& -> stays int&&

    std::cout << "template<typename T> void f(T&& arg);\n";
    std::cout << "f(lvalue);    // T deduced as int&, T&& & -> int&\n";
    std::cout << "f(rvalue);    // T deduced as int, T&& && -> int&&\n";

    std::cout << "\n";
}

void test_lifetime_extension() {
    std::cout << "=== Lifetime Extension with const lvalue ref ===\n";

    const int& r = 42;  // Temporary int(42) lifetime extended to r's scope
    std::cout << "const int& r = 42;  // Temporary lifetime extended\n";
    std::cout << "r is valid here: " << r << "\n";

    std::cout << "\n";
}

int main() {
    std::cout << "========== Reference Binding Demo ==========\n\n";
    test_lvalue_ref_binding();
    test_rvalue_ref_binding();
    test_function_params();
    test_reference_collapsing();
    test_lifetime_extension();
    std::cout << "All reference binding rules demonstrated.\n";
    return 0;
}
