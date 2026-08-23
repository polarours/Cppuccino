// C++23 std::print 完整示例
// 展示 std::print 的各种用法和特性

#include <iostream>
#include <print>
#include <string>
#include <vector>
#include <format>
#include <numeric>

// 自定义类型的格式化
struct Point {
    int x;
    int y;
};

// C++23 自定义 formatter
template<>
struct std::formatter<Point> : std::formatter<string> {
    auto format(const Point& p, format_context& ctx) const {
        return format_to(ctx.out(), "({}, {})", p.x, p.y);
    }
};

int main() {
    std::print("=== C++23 std::print Demo ===\n\n");
    
    // 1. 基本输出
    std::print("1. Basic output:\n");
    std::print("Hello, World!\n");
    std::print("Number: {}, Float: {}, String: {}\n", 42, 3.14, "test");
    
    // 2. 格式化说明
    std::print("\n2. Formatting:\n");
    std::print("Integer: {:d}\n", 42);
    std::print("Float: {:.2f}\n", 3.14159);
    std::print("String: {:<10}\n", "left");
    std::print("String: {:>10}\n", "right");
    std::print("String: {:^10}\n", "center");
    std::print("Binary: {:b}\n", 42);
    std::print("Hex: {:x}\n", 255);
    std::print("Pad zero: {:05d}\n", 42);
    
    // 3. 类型安全
    std::print("\n3. Type safety:\n");
    std::print("Type: {}\n", 42);  // 自动推导类型
    
    // 4. 自定义对象
    std::print("\n4. Custom object:\n");
    Point p{10, 20};
    std::print("Point: {}\n", p);
    
    // 5. 容器输出
    std::print("\n5. Container output:\n");
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::print("Vector: [");
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) std::print(", ");
        std::print("{}", vec[i]);
    }
    std::print("]\n");
    
    // 6. 与 std::format 配合
    std::print("\n6. Combined with std::format:\n");
    std::string formatted = std::format("Formatted: {} + {} = {}", 10, 20, 30);
    std::print("{}", formatted);
    
    // 7. 错误处理
    std::print("\n7. Error handling:\n");
    try {
        std::print("Value: {}\n", 42);
    } catch (const std::format_error& e) {
        std::print("Format error: {}\n", e.what());
    }
    
    std::print("\n=== Demo Complete ===\n");
    return 0;
}
