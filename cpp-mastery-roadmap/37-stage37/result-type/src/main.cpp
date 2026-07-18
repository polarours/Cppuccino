#include "result_type.hpp"

#include <iostream>

int main() {
    std::cout << "=== Result Type Demo ===\n\n";

    auto r1 = result_type::parseInt("42");
    std::cout << "Parse 42: " << (r1.isSuccess() ? "OK" : "FAIL") << " -> " << r1.valueOr(0) << "\n";

    auto r2 = result_type::parseInt("abc");
    std::cout << "Parse abc: " << (r2.isSuccess() ? "OK" : "FAIL") << " -> " << r2.error().message() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
