// examples/views-chunk-split.cpp
// Demonstrates C++23 views::chunk and views::split: batch and tokenize
// ranges lazily without manual index bookkeeping.
// Compile: g++ -std=c++23 -Wall -Wextra -o views-chunk-split views-chunk-split.cpp

#include <iostream>
#include <ranges>
#include <string>
#include <vector>

namespace chunk_split_demo {

// Flatten a chunked view into a printable summary line
std::string chunkSummary(const std::vector<int>& data, std::size_t n) {
    std::string out;
    for (auto chunk : data | std::views::chunk(n)) {
        out += "[";
        bool first = true;
        for (int v : chunk) {
            if (!first) out += ",";
            out += std::to_string(v);
            first = false;
        }
        out += "] ";
    }
    return out;
}

} // namespace chunk_split_demo

int main() {
    using namespace chunk_split_demo;
    std::cout << "=== C++23 views::chunk / views::split Demo ===\n\n";

    // 1. chunk: split into fixed-size batches (last chunk may be smaller)
    std::cout << "1. views::chunk(3) over 1..10:\n  ";
    for (auto chunk : std::views::iota(1, 11) | std::views::chunk(3)) {
        std::cout << '[';
        bool first = true;
        for (int v : chunk) {
            if (!first) std::cout << ',';
            std::cout << v;
            first = false;
        }
        std::cout << "] ";
    }
    std::cout << "\n";

    // 2. chunk on a real container
    std::cout << "\n2. chunk a vector into pairs:\n  ";
    std::vector<int> nums{10, 20, 30, 40, 50};
    std::cout << chunkSummary(nums, 2) << "\n";

    // 3. split: tokenize a string lazily
    std::cout << "\n3. views::split(',') on \"red,green,blue\":\n  ";
    std::string csv = "red,green,blue";
    for (auto token : csv | std::views::split(',')) {
        std::cout << '"';
        for (char ch : token) std::cout << ch;
        std::cout << "\" ";
    }
    std::cout << "\n";

    // 4. split by a predicate (split on whitespace)
    std::cout << "\n4. views::split on whitespace:\n  ";
    std::string line = "the quick brown fox";
    for (auto token : line | std::views::split(' ')) {
        std::cout << '[';
        for (char ch : token) std::cout << ch;
        std::cout << "] ";
    }
    std::cout << "\n";

    // 5. Both are lazy - compose with transform/filter
    std::cout << "\n5. chunk(2) then sum each chunk:\n  ";
    auto data = std::views::iota(1, 9);  // 1..8
    for (auto chunk : data | std::views::chunk(2)) {
        int sum = 0;
        for (int v : chunk) sum += v;
        std::cout << sum << ' ';
    }
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
