#include "zip_range.hpp"

#include <iostream>

int main() {
    std::cout << "=== Zip Range Demo ===\n\n";

    std::vector<int> ids = {1, 2, 3};
    std::vector<std::string> names = {"Alice", "Bob", "Charlie"};
    std::vector<double> scores = {95.5, 87.3, 92.1};

    for (auto [id, name, score] : zip_range::zip(ids, names, scores)) {
        std::cout << "#" << id << " " << name << ": " << score << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
