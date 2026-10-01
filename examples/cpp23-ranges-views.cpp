// examples/cpp23-ranges-views.cpp
// Demonstrates C++23 additions to <ranges>: views::zip, views::enumerate,
// ranges::to, and views::as_rvalue.
// Compile: g++ -std=c++23 -Wall -Wextra -o cpp23-ranges-views cpp23-ranges-views.cpp

#include <iostream>
#include <ranges>
#include <string>
#include <vector>

namespace ranges23_demo {

// ranges::to collects any view into a container - no manual loop
std::vector<int> evensUpTo(int n) {
    return std::views::iota(1, n + 1)
         | std::views::filter([](int x) { return x % 2 == 0; })
         | std::ranges::to<std::vector<int>>();
}

} // namespace ranges23_demo

int main() {
    using namespace ranges23_demo;
    std::cout << "=== C++23 Ranges Additions Demo ===\n\n";

    // 1. views::zip - pair two ranges lazily
    std::cout << "1. views::zip (names x scores):\n";
    std::vector<std::string> names{"alice", "bob", "carol"};
    std::vector<int> scores{91, 87, 95};
    for (const auto& [name, score] : std::views::zip(names, scores))
        std::cout << "  " << name << ": " << score << "\n";

    // 2. views::enumerate - index + value without manual counter
    std::cout << "\n2. views::enumerate:\n";
    for (const auto& [idx, name] : std::views::enumerate(names))
        std::cout << "  [" << idx << "] " << name << "\n";

    // 3. ranges::to - terminal operation, composes with any pipeline
    std::cout << "\n3. ranges::to<std::vector>:\n";
    auto even = evensUpTo(10);
    std::cout << "  evens up to 10:";
    for (int v : even) std::cout << ' ' << v;
    std::cout << "\n";

    // 4. views::as_rvalue - turn lvalue references into rvalue refs
    std::cout << "\n4. views::as_rvalue (move out of a vector):\n";
    std::vector<std::string> words{"alpha", "beta", "gamma"};
    std::vector<std::string> moved = std::move(words)
                                   | std::views::as_rvalue
                                   | std::ranges::to<std::vector<std::string>>();
    for (const auto& w : moved) std::cout << "  moved: " << w << "\n";

    // 5. Chained lazy evaluation - pipeline runs in one pass
    std::cout << "\n5. Chained pipeline (take first 3 squares of odds):\n";
    auto pipeline = std::views::iota(1)
                  | std::views::filter([](int x) { return x % 2 != 0; })
                  | std::views::transform([](int x) { return x * x; })
                  | std::views::take(3);
    for (int v : pipeline) std::cout << "  " << v << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
