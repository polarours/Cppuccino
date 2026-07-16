#include "sfinae_pattern.hpp"

#include <iostream>
#include <vector>

struct Widget {};

int main() {
    std::cout << "=== SFINAE Pattern Demo ===\n\n";

    std::cout << "int: " << sfinae_pattern::safePrint(42) << "\n";
    std::cout << "string: " << sfinae_pattern::safePrint(std::string("hello")) << "\n";
    std::cout << "Widget: " << sfinae_pattern::safePrint(Widget()) << "\n";

    std::vector<int> v = {1, 2, 3};
    std::cout << "vector size: " << sfinae_pattern::getSize(v) << "\n";
    std::cout << "int size: " << sfinae_pattern::getSize(42) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
