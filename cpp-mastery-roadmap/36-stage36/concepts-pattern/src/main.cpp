#include "concepts_pattern.hpp"

#include <iostream>
#include <vector>

int main() {
    std::cout << "=== Concepts Pattern Demo ===\n\n";

    std::vector<int> values = {1, 2, 3, 4, 5};
    std::cout << "Sum: " << concepts_pattern::sum(values) << "\n";
    std::cout << "Average: " << concepts_pattern::average(values) << "\n";

    concepts_pattern::Point p(3.0, 4.0);
    std::cout << "Point: " << concepts_pattern::describe(p) << "\n";

    concepts_pattern::printContainer(values);

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
