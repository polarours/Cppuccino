#include <flat_map>
#include <flat_set>
#include <iostream>
#include <string>
#include <vector>

// std::flat_map, std::flat_set (C++23)
// <flat_map>

int main() {
    std::cout << "=== std::flat_map & std::flat_set (C++23) ===\n\n";

    // std::flat_map: sorted key-value pairs (cache-friendly)
    std::flat_map<std::string, int> ages = {
        {"Alice", 30},
        {"Bob", 25},
        {"Charlie", 35}
    };

    std::cout << "Ages:\n";
    for (const auto& [name, age] : ages) {
        std::cout << "  " << name << ": " << age << "\n";
    }

    // Sorted by key
    std::cout << "\nFirst key: " << ages.begin()->first << "\n";  // Alice

    // Insert
    ages.insert({"Diana", 28});
    ages.insert({"Eve", 22});

    std::cout << "Size after inserts: " << ages.size() << "\n";

    // Lookup
    auto it = ages.find("Charlie");
    if (it != ages.end()) {
        std::cout << "Found Charlie: " << it->second << "\n";
    }

    // std::flat_set: sorted unique values
    std::flat_set<int> numbers = {5, 3, 8, 1, 9, 3, 5};

    std::cout << "\nflat_set (sorted, unique): ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";  // 1 3 5 8 9

    // Insert
    numbers.insert(7);
    numbers.insert(2);

    std::cout << "After inserts: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";  // 1 2 3 5 7 8 9

    // Comparison with std::map
    std::cout << "\nflat_map vs std::map:\n";
    std::cout << "  flat_map: contiguous storage, cache-friendly\n";
    std::cout << "  std::map: node-based, pointer chasing\n";
    std::cout << "  flat_map: O(log n) lookup, O(n) insert\n";
    std::cout << "  std::map: O(log n) lookup, O(log n) insert\n";

    return 0;
}
