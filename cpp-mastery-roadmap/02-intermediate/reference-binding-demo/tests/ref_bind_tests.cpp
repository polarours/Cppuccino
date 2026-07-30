#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>  // for std::move

// Template function for perfect forwarding test (outside any function)
template<typename T>
void wrapper(T&& arg) {
    (void)arg;  // suppress unused warning
}

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_lvalue_ref_accepts_lvalue() {
    int x = 42;
    int& r = x;  // Should compile
    expect(r == 42, "lvalue ref should bind to lvalue");
}

void test_const_lvalue_ref_accepts_lvalue() {
    int x = 42;
    const int& r = x;  // Should compile
    expect(r == 42, "const lvalue ref should bind to lvalue");
}

void test_const_lvalue_ref_accepts_rvalue() {
    const int& r = 42;  // Should compile (lifetime extended)
    expect(r == 42, "const lvalue ref should bind to rvalue");
}

void test_rvalue_ref_accepts_rvalue() {
    int&& r = 42;  // Should compile
    expect(r == 42, "rvalue ref should bind to rvalue");
}

void test_rvalue_ref_accepts_moved_lvalue() {
    int x = 42;
    int&& r = std::move(x);  // Should compile
    expect(r == 42, "rvalue ref should accept moved lvalue");
}

void test_template_forwarding() {
    int x = 42;
    wrapper(x);          // Should compile (lvalue)
    wrapper(std::move(x));  // Should compile (rvalue)
    wrapper(42);         // Should compile (rvalue literal)
    expect(true, "template forwarding should compile with lvalue and rvalue");
}

int main() {
    try {
        test_lvalue_ref_accepts_lvalue();
        test_const_lvalue_ref_accepts_lvalue();
        test_const_lvalue_ref_accepts_rvalue();
        test_rvalue_ref_accepts_rvalue();
        test_rvalue_ref_accepts_moved_lvalue();
        test_template_forwarding();
    } catch (const std::exception& e) {
        std::cerr << "ref_bind_tests failed: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "ref_bind_tests passed" << std::endl;
    return 0;
}
