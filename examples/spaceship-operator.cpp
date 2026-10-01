// examples/spaceship-operator.cpp
// Demonstrates C++20 three-way comparison (operator<=>): defaulted
// lexicographic ordering, custom ordering, and rewritten comparison rules.
// Compile: g++ -std=c++20 -Wall -Wextra -o spaceship-operator spaceship-operator.cpp

#include <algorithm>
#include <compare>
#include <iostream>
#include <string>
#include <vector>

namespace spaceship_demo {

// Defaulted <=> gives a *strong* ordering for all members at once.
struct Point {
    int x;
    int y;
    auto operator<=>(const Point&) const = default;  // no manual < <= > >= needed
    bool operator==(const Point&) const = default;
};

// Custom <=>: score then title (lexicographic by two keys)
struct Rating {
    std::string title;
    double score;

    std::strong_ordering operator<=>(const Rating& other) const {
        if (auto cmp = (score > other.score) - (score < other.score); cmp != 0)
            return cmp <=> 0;  // double<=>double is only partial_ordering; narrow it
        return title <=> other.title;
    }
    bool operator==(const Rating& other) const = default;
};

// Free defaulted <=> via hidden friend
struct Celsius {
    double degrees;
    Celsius(double d) : degrees(d) {}          // non-explicit: enables rewritten rules
    friend auto operator<=>(const Celsius& a, const Celsius& b) = default;
};

// Mixed comparison against a literal: `c > threshold` is rewritten as
// `threshold < c`, which converts the double via Celsius(double).
bool warmer(const Celsius& c, double threshold) {
    return c > threshold;
}

} // namespace spaceship_demo

int main() {
    using namespace spaceship_demo;
    std::cout << "=== C++20 operator<=> Demo ===\n\n";

    Point a{1, 2}, b{1, 3};
    std::cout << "Point(1,2) <=> Point(1,3): "
              << ((a <=> b) < 0 ? "< 0" : ">= 0") << "\n";
    std::cout << "  a < b :  " << std::boolalpha << (a < b) << "\n";
    std::cout << "  a > b :  " << (a > b) << "\n";
    std::cout << "  a == b:  " << (a == b) << "\n";

    std::cout << "\nLexicographic: members compared in declaration order (x then y)\n";
    Point c{2, 0}, d{1, 9};
    std::cout << "  Point(2,0) < Point(1,9): " << (c < d)
              << "  (x decides first)\n";

    std::cout << "\nSorting a vector needs only operator<=>:\n";
    std::vector<Point> pts{{3, 1}, {1, 5}, {2, 2}, {1, 1}};
    std::sort(pts.begin(), pts.end());
    for (const auto& p : pts)
        std::cout << "  (" << p.x << ", " << p.y << ")\n";

    std::cout << "\nCustom ordering (score then title):\n";
    Rating r1{"bbb", 4.5}, r2{"aaa", 4.5}, r3{"zzz", 4.9};
    std::cout << "  r1 < r2 : " << (r1 < r2) << "   (same score, title decides)\n";
    std::cout << "  r3 > r1 : " << (r3 > r1) << "   (higher score wins)\n";

    std::cout << "\nMixed comparison with double literal (rewritten rule):\n";
    Celsius body{36.6};
    std::cout << "  warmer(36.6, 36.0) = " << warmer(body, 36.0) << "\n";
    std::cout << "  warmer(36.6, 37.0) = " << warmer(body, 37.0) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
