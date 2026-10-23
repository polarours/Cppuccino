// examples/std-span-complete.cpp
// Demonstrates std::span: views over contiguous sequences.
// Compile with: g++ -std=c++20 -o std_span_complete std-span-complete.cpp


#include <iostream>
#include <span>
#include <vector>
#include <array>
#include <string>
#include <numeric>
#include <algorithm>

// Using a span instead of a raw pointer
void processSpan(std::span<const int> data) {
    std::cout << "Processing span of size: " << data.size() << "\n";
    std::cout << "First: " << data.front() << ", Last: " << data.back() << "\n";
    
    // Walking over it
    for (int v : data) {
        std::cout << v << " ";
    }
    std::cout << "\n";
}

// Working with a sub span
void processSubSpan(std::span<int> data, int start, int count) {
    auto sub = data.subspan(start, count);
    std::cout << "Sub-span: ";
    for (int v : sub) {
        std::cout << v << " ";
    }
    std::cout << "\n";
}

// A two dimensional span
void process2DSpan(std::span<std::span<const int>> matrix) {
    std::cout << "2D matrix:\n";
    for (const auto& row : matrix) {
        for (int v : row) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
}

// A span over a string
void processStringSpan(std::span<const char> str) {
    std::cout << "String span: ";
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != '\0') std::cout << str[i];
    }
    std::cout << "\n";
}

int main() {
    std::cout << "=== C++20 std::span Demo ===\n\n";
    
    // 1. The basic use
    std::cout << "1. Basic span:\n";
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::span<int> span1(vec);
    std::cout << "Span from vector: ";
    for (int v : span1) std::cout << v << " ";
    std::cout << "\n";
    
    // 2. An array
    std::cout << "\n2. Array span:\n";
    std::array<int, 5> arr = {10, 20, 30, 40, 50};
    std::span<int, 5> span2(arr);
    std::cout << "Span from array: ";
    for (int v : span2) std::cout << v << " ";
    std::cout << "\n";
    
    // 3. A raw array
    std::cout << "\n3. Raw array span:\n";
    int raw[] = {100, 200, 300};
    std::span<int> span3(raw);
    std::cout << "Span from raw array: ";
    for (int v : span3) std::cout << v << " ";
    std::cout << "\n";
    
    // 4. A function parameter
    std::cout << "\n4. Function parameters:\n";
    processSpan(vec);
    
    // 5. A sub span
    std::cout << "\n5. Sub-span:\n";
    processSubSpan(vec, 1, 3);
    
    // 6. A two dimensional span
    std::cout << "\n6. 2D span:\n";
    std::vector<std::vector<int>> matrix = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    std::vector<std::span<const int>> spans;
    for (auto& row : matrix) {
        spans.emplace_back(row);
    }
    process2DSpan(spans);
    
    // 7. A span over a string
    std::cout << "\n7. String span:\n";
    std::string str = "Hello, C++20!";
    std::span<const char> strSpan(str.data(), str.size());
    processStringSpan(strSpan);
    
    // 8. Working out the type
    std::cout << "\n8. Type deduction:\n";
    auto span4 = std::span<const int>(vec);
    std::cout << "Deduced span size: " << span4.size() << "\n";
    
    // 9. A read only span
    std::cout << "\n9. Read-only span:\n";
    const std::vector<int> cvec = {1, 2, 3};
    std::span<const int> cspan(cvec);
    // cspan[0] = 10; // error: it cannot be modified
    std::cout << "Read-only span: ";
    for (int v : cspan) std::cout << v << " ";
    std::cout << "\n";
    
    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
