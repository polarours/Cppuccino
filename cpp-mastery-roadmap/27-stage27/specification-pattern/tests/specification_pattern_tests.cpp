#include "specification_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_active_spec() {
    auto spec = specification_pattern::makeActiveSpec();
    specification_pattern::Product active("A", 10, 5, true);
    specification_pattern::Product inactive("B", 10, 5, false);

    expect(spec.isSatisfiedBy(active), "expected active");
    expect(!spec.isSatisfiedBy(inactive), "expected inactive rejected");
}

void test_price_below() {
    auto spec = specification_pattern::makePriceBelowSpec(50.0);
    specification_pattern::Product cheap("A", 30, 1, true);
    specification_pattern::Product expensive("B", 100, 1, true);

    expect(spec.isSatisfiedBy(cheap), "expected cheap satisfied");
    expect(!spec.isSatisfiedBy(expensive), "expected expensive rejected");
}

void test_and_combination() {
    auto active = specification_pattern::makeActiveSpec();
    auto inStock = specification_pattern::makeInStockSpec();
    auto combined = active.andSpec(inStock);

    specification_pattern::Product p1("A", 10, 5, true);
    specification_pattern::Product p2("B", 10, 0, true);
    specification_pattern::Product p3("C", 10, 5, false);

    expect(combined.isSatisfiedBy(p1), "expected active+inStock");
    expect(!combined.isSatisfiedBy(p2), "expected out-of-stock rejected");
    expect(!combined.isSatisfiedBy(p3), "expected inactive rejected");
}

void test_or_combination() {
    auto cheap = specification_pattern::makePriceBelowSpec(20.0);
    auto expensive = specification_pattern::makePriceBelowSpec(200.0);
    auto combined = cheap.orSpec(expensive);

    specification_pattern::Product mid("A", 100, 1, true);
    expect(combined.isSatisfiedBy(mid), "expected mid satisfies or");
}

void test_not_combination() {
    auto active = specification_pattern::makeActiveSpec();
    auto notActive = active.notSpec();

    specification_pattern::Product p1("A", 10, 5, true);
    specification_pattern::Product p2("B", 10, 5, false);

    expect(!notActive.isSatisfiedBy(p1), "expected active rejected by NOT");
    expect(notActive.isSatisfiedBy(p2), "expected inactive accepted by NOT");
}

void test_filter() {
    std::vector<specification_pattern::Product> products = {
        {"Laptop", 999, 5, true},
        {"Mouse", 25, 10, true},
        {"Old Phone", 50, 0, false},
    };

    auto spec = specification_pattern::makeActiveSpec();
    auto results = specification_pattern::filter(products, spec);
    expect(results.size() == 2, "expected 2 active products");
}

} // namespace

int main() {
    try {
        std::cout << "Specification Pattern Tests:\n";
        test_active_spec();
        std::cout << "  test_active_spec: PASS\n";

        test_price_below();
        std::cout << "  test_price_below: PASS\n";

        test_and_combination();
        std::cout << "  test_and_combination: PASS\n";

        test_or_combination();
        std::cout << "  test_or_combination: PASS\n";

        test_not_combination();
        std::cout << "  test_not_combination: PASS\n";

        test_filter();
        std::cout << "  test_filter: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "specification_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nspecification_pattern_tests passed" << std::endl;
    return 0;
}
