// C++23 特性完整示例
#include <iostream>
#include <print>
#include <concepts>
#include <expected>
#include <flat_map>
#include <mdspan>

int main() {
    std::print("=== C++23 Features Demo ===\n\n");
    
    // 1. std::print
    std::print("1. std::print:\n");
    std::print("Hello, C++23!\n");
    std::print("Number: {}\n", 42);
    
    // 2. std::flat_map (有序哈希表)
    std::print("\n2. std::flat_map:\n");
    std::flat_map<std::string, int> scores = {
        {"Alice", 95}, {"Bob", 87}, {"Charlie", 92}
    };
    for (const auto& [name, score] : scores) {
        std::print("  {}: {}\n", name, score);
    }
    
    // 3. std::mdspan (多维数组视图)
    std::print("\n3. std::mdspan:\n");
    std::vector<int> data = {1, 2, 3, 4, 5, 6};
    std::mdspan<int, std::extents<std::size_t, 2, 3>> matrix(data.data(), 2, 3);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::print("{} ", matrix.at(i, j));
        }
        std::print("\n");
    }
    
    // 4. std::expected (C++23)
    std::print("\n4. std::expected:\n");
    std::expected<int, std::string> result = 42;
    if (result) {
        std::print("  Result: {}\n", *result);
    }
    
    std::print("\n=== Demo Complete ===\n");
    return 0;
}
