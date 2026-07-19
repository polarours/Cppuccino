#include <iostream>
#include <string>

// --- basic fold ---
template <typename... Args>
auto sum(Args... args) {
    return (args + ...);
}

// --- right fold ---
template <typename... Args>
void printAll(Args... args) {
    (std::cout << ... << args) << "\n";
}

// --- unary fold ---
template <typename... Args>
bool allTrue(Args... args) {
    return (... && args);
}

// --- nested fold ---
template <typename... Args>
auto multiplyAll(Args... args) {
    return (args * ... * 1);
}

// --- string concat ---
std::string concat(const std::string& separator, const std::string& first) {
    return first;
}

template <typename... Rest>
std::string concat(const std::string& separator, const std::string& first, const Rest&... rest) {
    return first + separator + concat(separator, rest...);
}

int main() {
    std::cout << "=== Fold Expressions ===\n\n";

    std::cout << "sum(1,2,3,4,5) = " << sum(1, 2, 3, 4, 5) << "\n";
    std::cout << "multiplyAll(2,3,4) = " << multiplyAll(2, 3, 4) << "\n";

    std::cout << "printAll: ";
    printAll("Hello", " ", "World", "!");

    std::cout << "allTrue(true,true,true) = " << std::boolalpha << allTrue(true, true, true) << "\n";
    std::cout << "allTrue(true,false,true) = " << allTrue(true, false, true) << "\n";

    std::cout << "concat: " << concat(", ", "a", "b", "c", "d") << "\n";

    return 0;
}
