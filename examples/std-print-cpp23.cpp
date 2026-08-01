#include <iostream>
#include <vector>
#include <algorithm>
#include <ranges>
#include <format>

// C++23 std::print alternative using std::format
// Demonstrates modern formatting patterns

int main() {
    std::cout << "=== Modern Formatting Demo ===\n\n";

    // Basic formatting
    std::cout << "1. Basic output:\n";
    std::string name = "Alice";
    int age = 30;
    std::cout << "  Name: " << name << ", Age: " << age << "\n";

    // String formatting with std::format
    std::cout << "\n2. std::format:\n";
    std::string greeting = std::format("Hello, {}! You are {} years old.", name, age);
    std::cout << "  " << greeting << "\n";

    // Container output
    std::cout << "\n3. Container output:\n";
    std::vector<int> nums = {1, 2, 3, 4, 5};
    std::cout << "  Vector: [";
    for (size_t i = 0; i < nums.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << nums[i];
    }
    std::cout << "]\n";

    // Range-based formatting
    std::cout << "\n4. Range operations:\n";
    std::vector<std::string> words = {"hello", "world", "cpp"};
    auto upper = words;
    for (auto& w : upper) {
        std::ranges::transform(w, w.begin(), ::toupper);
    }
    std::cout << "  Upper: ";
    for (const auto& w : upper) std::cout << w << " ";
    std::cout << "\n";

    // Filter and transform
    std::cout << "\n5. Filter + Transform:\n";
    auto longWords = words | std::views::filter([](const std::string& s) {
                       return s.size() > 4;
                   });
    for (const auto& w : longWords) std::cout << "  " << w << " ";
    std::cout << "\n";

    return 0;
}
