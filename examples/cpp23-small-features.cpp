// examples/cpp23-small-features.cpp
// Demonstrates smaller C++23 library features: std::string::contains,
// std::to_underlying, std::byteswap, and std::expected value_or.
// Compile: g++ -std=c++23 -Wall -Wextra -o cpp23-small-features cpp23-small-features.cpp

#include <bit>
#include <expected>
#include <iostream>
#include <string>
#include <utility>

namespace cpp23_small {

enum class Level { debug = 1, info = 2, error = 3 };

// to_underlying: enum -> underlying integer without a cast
int code(Level l) { return std::to_underlying(l); }

std::expected<int, std::string> parseInt(const std::string& s) {
    try {
        size_t pos = 0;
        int v = std::stoi(s, &pos);
        if (pos != s.size()) return std::unexpected("trailing chars: " + s);
        return v;
    } catch (...) {
        return std::unexpected("not an int: " + s);
    }
}

} // namespace cpp23_small

int main() {
    using namespace cpp23_small;
    std::cout << "=== C++23 Small Library Features Demo ===\n\n";

    // 1. std::string::contains - no more find() != npos
    std::cout << "1. string::contains:\n";
    std::string email = "alice@example.com";
    std::cout << "  \"" << email << "\" contains '@'      : "
              << email.contains('@') << "\n";
    std::cout << "  contains \"example\"              : "
              << email.contains("example") << "\n";
    std::cout << "  contains \"gmail\"                 : "
              << email.contains("gmail") << "\n";

    // 2. std::to_underlying - enum to its storage type
    std::cout << "\n2. to_underlying:\n";
    std::cout << "  Level::debug = " << code(Level::debug) << "\n";
    std::cout << "  Level::error = " << code(Level::error) << "\n";

    // 3. std::byteswap - byte order reversal
    std::cout << "\n3. byteswap:\n";
    unsigned int v = 0x0A0B0C0D;
    std::cout << "  0x0A0B0C0D -> 0x" << std::hex << std::byteswap(v)
              << std::dec << "\n";
    std::cout << "  swap twice returns original: "
              << (std::byteswap(std::byteswap(v)) == v) << "\n";

    // 4. expected::value_or - default when the computation failed
    std::cout << "\n4. expected value_or:\n";
    for (const char* in : {"42", "abc", "7x"}) {
        auto r = parseInt(in);
        std::cout << "  parseInt(\"" << in << "\") = "
                  << r.value_or(-1);
        if (!r) std::cout << "  (error: " << r.error() << ")";
        std::cout << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
