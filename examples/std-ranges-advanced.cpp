#include <vector>
#include <string>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <numeric>

// Advanced ranges demonstration
// Shows composition of views, filters, and transformations

int main() {
    std::cout << "=== Advanced Ranges Demo (C++20) ===\n\n";

    std::vector<std::string> words = {"hello", "world", "cpp", "ranges",
                                       "advanced", "example", "test"};

    // Range-based algorithms
    std::cout << "1. Range algorithms:\n";
    auto sorted = words;
    std::ranges::sort(sorted);
    for (const auto& w : sorted) std::cout << "  " << w << " ";
    std::cout << "\n";

    // Chained views: filter + transform
    std::cout << "\n2. Filter + Transform:\n";
    auto longWords = words | std::views::filter([](const std::string& s) {
                       return s.size() > 4;
                   }) | std::views::transform([](const std::string& s) {
                       std::string reversed = s;
                       std::ranges::reverse(reversed);
                       return reversed;
                   });
    for (const auto& w : longWords) std::cout << "  " << w << " ";
    std::cout << "\n";

    // Take view
    std::cout << "\n3. Take (first 3 words):\n";
    auto taken = words | std::views::take(3);
    for (const auto& w : taken) std::cout << "  " << w << " ";
    std::cout << "\n";

    // Drop view
    std::cout << "\n4. Drop (skip first 2):\n";
    auto dropped = words | std::views::drop(2);
    for (const auto& w : dropped) std::cout << "  " << w << " ";
    std::cout << "\n";

    // Reverse view
    std::cout << "\n5. Reverse:\n";
    auto reversed = words | std::views::reverse;
    for (const auto& w : reversed) std::cout << "  " << w << " ";
    std::cout << "\n";

    // Common range operations
    std::cout << "\n6. Common operations:\n";
    std::vector<int> values = {5, 2, 8, 1, 9, 3};
    std::cout << "  Sum: " << std::accumulate(values.begin(), values.end(), 0) << "\n";
    std::cout << "  Max: " << *std::ranges::max_element(values) << "\n";
    std::cout << "  Min: " << *std::ranges::min_element(values) << "\n";
    std::cout << "  Reversed: ";
    for (int n : values | std::views::reverse) std::cout << n << " ";
    std::cout << "\n";

    // Unique with algorithm
    std::cout << "\n7. Unique (remove consecutive duplicates):\n";
    std::vector<int> nums = {1, 2, 2, 3, 3, 3, 4, 4, 5};
    auto unique = nums;
    auto last = std::unique(unique.begin(), unique.end());
    unique.erase(last, unique.end());
    for (int n : unique) std::cout << "  " << n;
    std::cout << "\n";

    // Drop while
    std::cout << "\n8. Drop while (skip short words):\n";
    auto noShort = words | std::views::drop_while([](const std::string& s) {
                       return s.size() < 5;
                   });
    for (const auto& w : noShort) std::cout << "  " << w << " ";
    std::cout << "\n";

    return 0;
}
