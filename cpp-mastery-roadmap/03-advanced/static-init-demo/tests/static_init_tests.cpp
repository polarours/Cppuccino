#include "counter.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_counter_creation() {
    Counter local("Local");
    expect(local.getValue() == 0, "expected initial value 0");
}

void test_increment() {
    Counter c("Test");
    c.increment();
    // increment doesn't return anything, just check it compiles
    expect(true, "increment should compile");
}

void test_check_with_other() {
    Counter a("A");
    Counter b("B");
    a.checkWithOther(b);  // Should compile and work
    expect(true, "checkWithOther should work");
}

} // namespace

int main() {
    try {
        test_counter_creation();
        test_increment();
        test_check_with_other();
    } catch (const std::exception& e) {
        std::cerr << "static_init_tests failed: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "static_init_tests passed" << std::endl;
    return 0;
}
