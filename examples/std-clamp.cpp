#include <algorithm>
#include <iostream>

// std::clamp - restricts a value to a range [lo, hi]
// C++17: <algorithm>

int main() {
    std::cout << "=== std::clamp (C++17) ===\n\n";

    // Basic usage
    int x = 150;
    int clamped = std::clamp(x, 0, 100);
    std::cout << "clamp(150, 0, 100) = " << clamped << "\n";  // 100

    x = 50;
    clamped = std::clamp(x, 0, 100);
    std::cout << "clamp(50, 0, 100) = " << clamped << "\n";    // 50

    x = -10;
    clamped = std::clamp(x, 0, 100);
    std::cout << "clamp(-10, 0, 100) = " << clamped << "\n";   // 0

    // Useful for normalizing values
    double brightness = 256.0;
    double normalized = std::clamp(brightness, 0.0, 255.0);
    std::cout << "\nBrightness 256 -> " << normalized << "\n";  // 255

    brightness = -50.0;
    normalized = std::clamp(brightness, 0.0, 255.0);
    std::cout << "Brightness -50 -> " << normalized << "\n";    // 0

    // Works with any comparable type
    std::string name = "Alphabet";
    std::string clampedName = std::clamp(name, std::string("A"), std::string("M"));
    std::cout << "\nclamp(\"Alphabet\", \"A\", \"M\") = " << clampedName << "\n";  // "Alphabet"

    name = "Zebra";
    clampedName = std::clamp(name, std::string("A"), std::string("M"));
    std::cout << "clamp(\"Zebra\", \"A\", \"M\") = " << clampedName << "\n";      // "M"

    // Equivalent to: std::max(lo, std::min(hi, value))
    // But clamp is clearer and avoids double evaluation

    return 0;
}
