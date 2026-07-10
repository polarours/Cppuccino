#include "factory_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_create_book() {
    auto product = factory_pattern::ProductFactory::create("book", "C++ Primer", 49.99);
    expect(product->name() == "Book: C++ Primer", "expected book name");
    expect(product->price() == 49.99, "expected book price");
}

void test_create_electronics() {
    auto product = factory_pattern::ProductFactory::create("electronics", "Laptop", 999.99);
    expect(product->name() == "Electronics: Laptop", "expected electronics name");
    expect(product->price() == 999.99, "expected electronics price");
}

void test_create_clothing() {
    auto product = factory_pattern::ProductFactory::create("clothing", "T-Shirt", 19.99);
    expect(product->name() == "Clothing: T-Shirt", "expected clothing name");
    expect(product->price() == 19.99, "expected clothing price");
}

void test_unknown_type() {
    bool threw = false;
    try {
        factory_pattern::ProductFactory::create("unknown", "Test", 9.99);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "expected exception for unknown type");
}

void test_different_products() {
    auto book = factory_pattern::ProductFactory::create("book", "Book1", 10.0);
    auto electronics = factory_pattern::ProductFactory::create("electronics", "Electronics1", 20.0);
    auto clothing = factory_pattern::ProductFactory::create("clothing", "Clothing1", 30.0);

    expect(book->name().find("Book") != std::string::npos, "expected Book");
    expect(electronics->name().find("Electronics") != std::string::npos, "expected Electronics");
    expect(clothing->name().find("Clothing") != std::string::npos, "expected Clothing");
}

} // namespace

int main() {
    try {
        std::cout << "Factory Pattern Tests:\n";
        test_create_book();
        std::cout << "  test_create_book: PASS\n";

        test_create_electronics();
        std::cout << "  test_create_electronics: PASS\n";

        test_create_clothing();
        std::cout << "  test_create_clothing: PASS\n";

        test_unknown_type();
        std::cout << "  test_unknown_type: PASS\n";

        test_different_products();
        std::cout << "  test_different_products: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "factory_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nfactory_pattern_tests passed" << std::endl;
    return 0;
}
