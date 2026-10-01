// examples/inplace-vector.cpp
// Demonstrates std::inplace_vector (C++26): a vector with inline storage -
// no heap allocation, fixed capacity, same interface as std::vector.
// Compile: g++ -std=c++26 -Wall -Wextra -o inplace-vector inplace-vector.cpp

#include <inplace_vector>
#include <iostream>
#include <string>

namespace inplace_vector_demo {

// Capacity is part of the type: this vector NEVER allocates.
// (Unlike std::vector, different capacities are different types -
//  there is no implicit conversion between them.)
using SmallInts = std::inplace_vector<int, 8>;
using Names = std::inplace_vector<std::string, 4>;

template <std::size_t N>
int sum(const std::inplace_vector<int, N>& v) {
    int total = 0;
    for (int x : v) total += x;
    return total;
}

} // namespace inplace_vector_demo

int main() {
    using namespace inplace_vector_demo;
    std::cout << "=== std::inplace_vector (C++26) Demo ===\n\n";

    // 1. Basic usage - looks like std::vector, but inline storage
    std::cout << "1. push_back / iteration:\n";
    SmallInts v;
    for (int i = 1; i <= 5; ++i) v.push_back(i * 10);
    std::cout << "  size=" << v.size() << " capacity=" << v.capacity()
              << " contents:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << "\n";

    // 2. Capacity is a compile-time property
    std::cout << "\n2. compile-time capacity:\n";
    std::cout << "  SmallInts max size = " << SmallInts().max_size() << "\n";
    std::cout << "  is fully inline: no new/delete for these "
              << v.size() << " ints\n";

    // 3. push_back beyond capacity throws (bounded, unlike a raw array)
    std::cout << "\n3. overflow behavior:\n";
    std::inplace_vector<int, 2> tiny{1, 2};
    try {
        tiny.push_back(3);
    } catch (const std::exception& e) {
        std::cout << "  tiny (cap 2) push_back #3 -> throws: " << e.what() << "\n";
    }

    // 4. Works with non-trivial types (inline std::string array)
    std::cout << "\n4. non-trivial elements inline:\n";
    Names names;
    names.emplace_back("alice");
    names.emplace_back("bob");
    for (const auto& n : names) std::cout << "  " << n << "\n";

    // 5. sum() is templated on capacity - each capacity is its own type
    std::cout << "\n5. passed to a capacity-templated parameter:\n";
    std::inplace_vector<int, 4> fours{4, 8, 15, 16};
    std::cout << "  sum(fours) = " << sum(fours) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
