#include "crtp_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_dog_counter() {
    crtp_pattern::Dog::reset();
    crtp_pattern::Dog d1("Rex");
    crtp_pattern::Dog d2("Buddy");

    expect(crtp_pattern::Dog::count() == 2, "expected 2 dogs");
    expect(d1.bark() == "Rex says: Woof!", "expected bark");
}

void test_cat_independent() {
    crtp_pattern::Cat::reset();
    crtp_pattern::Cat c1("Whiskers");

    expect(crtp_pattern::Cat::count() == 1, "expected 1 cat");
}

void test_printable() {
    crtp_pattern::User user("Alice", 30);
    crtp_pattern::Product product("Widget", 9.99);

    expect(user.print().find("Alice") != std::string::npos, "expected Alice in user");
    expect(product.print().find("Widget") != std::string::npos, "expected Widget in product");
}

void test_counter_destructor() {
    crtp_pattern::Dog::reset();
    {
        crtp_pattern::Dog d1("Rex");
        crtp_pattern::Dog d2("Buddy");
        expect(crtp_pattern::Dog::count() == 2, "expected 2");
    }
    expect(crtp_pattern::Dog::count() == 0, "expected 0 after scope");
}

} // namespace

int main() {
    try {
        std::cout << "CRTP Pattern Tests:\n";
        test_dog_counter();
        std::cout << "  test_dog_counter: PASS\n";

        test_cat_independent();
        std::cout << "  test_cat_independent: PASS\n";

        test_printable();
        std::cout << "  test_printable: PASS\n";

        test_counter_destructor();
        std::cout << "  test_counter_destructor: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "crtp_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncrtp_pattern_tests passed" << std::endl;
    return 0;
}
