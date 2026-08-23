// C++20 std::span 完整示例
// 展示 span 的各种用法和性能优势

#include <iostream>
#include <span>
#include <vector>
#include <array>
#include <string>
#include <numeric>
#include <algorithm>

// 使用 span 替代原始指针
void processSpan(std::span<const int> data) {
    std::cout << "Processing span of size: " << data.size() << "\n";
    std::cout << "First: " << data.front() << ", Last: " << data.back() << "\n";
    
    // 遍历
    for (int v : data) {
        std::cout << v << " ";
    }
    std::cout << "\n";
}

// 子 span 操作
void processSubSpan(std::span<int> data, int start, int count) {
    auto sub = data.subspan(start, count);
    std::cout << "Sub-span: ";
    for (int v : sub) {
        std::cout << v << " ";
    }
    std::cout << "\n";
}

// 二维 span
void process2DSpan(std::span<std::span<const int>> matrix) {
    std::cout << "2D matrix:\n";
    for (const auto& row : matrix) {
        for (int v : row) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
}

// 字符串 span
void processStringSpan(std::span<const char> str) {
    std::cout << "String span: ";
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != '\0') std::cout << str[i];
    }
    std::cout << "\n";
}

int main() {
    std::cout << "=== C++20 std::span Demo ===\n\n";
    
    // 1. 基本用法
    std::cout << "1. Basic span:\n";
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::span<int> span1(vec);
    std::cout << "Span from vector: ";
    for (int v : span1) std::cout << v << " ";
    std::cout << "\n";
    
    // 2. 数组
    std::cout << "\n2. Array span:\n";
    std::array<int, 5> arr = {10, 20, 30, 40, 50};
    std::span<int, 5> span2(arr);
    std::cout << "Span from array: ";
    for (int v : span2) std::cout << v << " ";
    std::cout << "\n";
    
    // 3. 原始数组
    std::cout << "\n3. Raw array span:\n";
    int raw[] = {100, 200, 300};
    std::span<int> span3(raw);
    std::cout << "Span from raw array: ";
    for (int v : span3) std::cout << v << " ";
    std::cout << "\n";
    
    // 4. 函数参数
    std::cout << "\n4. Function parameters:\n";
    processSpan(vec);
    
    // 5. 子 span
    std::cout << "\n5. Sub-span:\n";
    processSubSpan(vec, 1, 3);
    
    // 6. 二维 span
    std::cout << "\n6. 2D span:\n";
    std::vector<std::vector<int>> matrix = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    std::vector<std::span<const int>> spans;
    for (auto& row : matrix) {
        spans.emplace_back(row);
    }
    process2DSpan(spans);
    
    // 7. 字符串 span
    std::cout << "\n7. String span:\n";
    std::string str = "Hello, C++20!";
    std::span<const char> strSpan(str.data(), str.size());
    processStringSpan(strSpan);
    
    // 8. 类型推导
    std::cout << "\n8. Type deduction:\n";
    auto span4 = std::span<const int>(vec);
    std::cout << "Deduced span size: " << span4.size() << "\n";
    
    // 9. 只读 span
    std::cout << "\n9. Read-only span:\n";
    const std::vector<int> cvec = {1, 2, 3};
    std::span<const int> cspan(cvec);
    // cspan[0] = 10; // 错误：不能修改
    std::cout << "Read-only span: ";
    for (int v : cspan) std::cout << v << " ";
    std::cout << "\n";
    
    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
