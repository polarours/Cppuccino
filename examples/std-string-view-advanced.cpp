#include <string_view>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

// std::string_view - non-owning view of string (C++17)
// <string_view>

// Pass by string_view to avoid copies
std::size_t countWords(std::string_view text) {
    std::size_t count = 0;
    bool inWord = false;

    for (char c : text) {
        if (std::isspace(c)) {
            inWord = false;
        } else if (!inWord) {
            inWord = true;
            count++;
        }
    }
    return count;
}

std::string_view trimLeft(std::string_view sv) {
    auto pos = sv.find_first_not_of(" \t\n\r");
    return (pos == std::string_view::npos) ? "" : sv.substr(pos);
}

std::string_view trimRight(std::string_view sv) {
    auto pos = sv.find_last_not_of(" \t\n\r");
    return (pos == std::string_view::npos) ? "" : sv.substr(0, pos + 1);
}

std::string_view trim(std::string_view sv) {
    return trimRight(trimLeft(sv));
}

// WARNING: dangling string_view
std::string_view dangerous() {
    std::string s = "temporary";
    return s;  // DANGER: s destroyed when function returns!
}

int main() {
    std::cout << "=== std::string_view (C++17) ===\n\n";

    // Basic usage
    std::string_view sv1 = "Hello, World!";
    std::cout << "sv1: " << sv1 << "\n";
    std::cout << "sv1.size(): " << sv1.size() << "\n";

    // From std::string
    std::string str = "Hello, World!";
    std::string_view sv2 = str;
    std::cout << "sv2 from string: " << sv2 << "\n";

    // Substring without allocation
    std::string_view sv3 = sv1.substr(0, 5);
    std::cout << "sv1.substr(0,5): " << sv3 << "\n";

    // Search
    auto pos = sv1.find("World");
    if (pos != std::string_view::npos) {
        std::cout << "Found 'World' at position: " << pos << "\n";
    }

    // Comparison
    std::cout << "\nComparison:\n";
    std::cout << "\"Hello\" == \"Hello\": " << (std::string_view("Hello") == std::string_view("Hello")) << "\n";
    std::cout << "\"Hello\" < \"World\": " << (std::string_view("Hello") < std::string_view("World")) << "\n";

    // Practical: word counting
    std::string_view text = "  The quick brown fox jumps over the lazy dog  ";
    std::cout << "\nText: '" << text << "'\n";
    std::cout << "Words: " << countWords(text) << "\n";

    // Trim
    std::cout << "Trimmed: '" << trim(text) << "'\n";
    std::cout << "Trimmed size: " << trim(text).size() << "\n";

    // No allocation overhead
    std::cout << "\nPerformance:\n";
    std::cout << "string_view: no heap allocation\n";
    std::cout << "string: allocates on heap for large strings\n";

    // WARNING example (commented out to prevent crash)
    // std::string_view bad = dangerous();
    // std::cout << bad << "\n";  // UNDEFINED BEHAVIOR

    return 0;
}
