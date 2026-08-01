#include <iostream>
#include <string>
#include <format>
#include <vector>

// std::format - C++20 type-safe formatting

int main() {
    std::cout << "=== std::format Demo (C++20) ===\n\n";

    // Basic formatting
    std::cout << "1. Basic formatting:\n";
    std::string name = "Alice";
    int age = 30;
    std::string greeting = std::format("Hello, {}! You are {} years old.", name, age);
    std::cout << "  " << greeting << "\n";

    // Numerical formatting
    std::cout << "\n2. Numerical formatting:\n";
    double pi = 3.14159265;
    std::cout << "  Pi = " << std::format("{:.2f}", pi) << " (2 decimal places)\n";
    std::cout << "  Pi = " << std::format("{:.6f}", pi) << " (6 decimal places)\n";
    std::cout << "  Pi = " << std::format("{:e}", pi) << " (scientific)\n";
    std::cout << "  Hex: " << std::format("{:x}", 255) << "\n";
    std::cout << "  Binary: " << std::format("{:08b}", 42) << " (zero-padded)\n";

    // Alignment
    std::cout << "\n3. Alignment:\n";
    std::cout << "  Left-aligned:  '" << std::format("{:<20}", "hello") << "'\n";
    std::cout << "  Right-aligned: '" << std::format("{:>20}", "hello") << "'\n";
    std::cout << "  Centered:      '" << std::format("{:^20}", "hello") << "'\n";

    // Positional arguments
    std::cout << "\n4. Positional arguments:\n";
    std::cout << "  {1} {0}: {1} is at position 0, {0} at position 1\n",
              "world", "hello";

    // Container formatting
    std::cout << "\n5. Container formatting:\n";
    std::vector<int> nums = {1, 2, 3, 4, 5};
    std::cout << "  Vector: [";
    for (size_t i = 0; i < nums.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << nums[i];
    }
    std::cout << "]\n";

    // Named placeholders
    std::cout << "\n6. Structured data:\n";
    struct Point { int x; int y; };
    Point p{3, 4};
    std::string pointStr = std::format("Point({}, {})", p.x, p.y);
    std::cout << "  " << pointStr << "\n";

    // Error handling
    std::cout << "\n7. Try formatting:\n";
    try {
        std::string result = std::format("Value: {}", 42);
        std::cout << "  " << result << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Format error: " << e.what() << "\n";
    }

    return 0;
}
