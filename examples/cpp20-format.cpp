// examples/cpp20-format.cpp
// Demonstrates C++20 std::format for string formatting.
// Compile with: g++ -std=c++20 -o cpp20_format cpp20-format.cpp

#include <iostream>
#include <format>
#include <string>
#include <chrono>

// --- 1. Basic Formatting ---

void basicFormatting() {
    std::cout << "--- Basic Formatting ---\n";

    std::string name = "Alice";
    int age = 25;
    double pi = 3.14159;

    std::cout << std::format("Name: {}, Age: {}\n", name, age);
    std::cout << std::format("Pi: {:.2f}\n", pi);
}

// --- 2. Position Arguments ---

void positionArguments() {
    std::cout << "\n--- Position Arguments ---\n";

    std::cout << std::format("{0} is {1}, {1} is {0}\n", "hello", "world");
    std::cout << std::format("{1} {0}\n", "second", "first");
}

// --- 3. Format Specifiers ---

void formatSpecifiers() {
    std::cout << "\n--- Format Specifiers ---\n";

    int num = 42;
    double pi = 3.14159265358979;

    // Integer formatting
    std::cout << std::format("Decimal: {}\n", num);
    std::cout << std::format("Hex: {:x}\n", num);
    std::cout << std::format("Hex (upper): {:X}\n", num);
    std::cout << std::format("Octal: {:o}\n", num);
    std::cout << std::format("Binary: {:b}\n", num);

    // Float formatting
    std::cout << std::format("Default: {}\n", pi);
    std::cout << std::format("Fixed: {:.2f}\n", pi);
    std::cout << std::format("Scientific: {:e}\n", pi);
    std::cout << std::format("General: {:g}\n", pi);
}

// --- 4. Width and Alignment ---

void widthAndAlignment() {
    std::cout << "\n--- Width and Alignment ---\n";

    int num = 42;
    std::string str = "hi";

    // Width
    std::cout << std::format("[{:10}]\n", num);
    std::cout << std::format("[{:10}]\n", str);

    // Alignment
    std::cout << std::format("[{:<10}]\n", str);   // Left
    std::cout << std::format("[{:>10}]\n", str);   // Right
    std::cout << std::format("[{:^10}]\n", str);   // Center

    // Fill character
    std::cout << std::format("[{:*^10}]\n", str);
    std::cout << std::format("[{:010}]\n", num);
}

// --- 5. Escape Sequences ---

void escapeSequences() {
    std::cout << "\n--- Escape Sequences ---\n";

    std::cout << std::format("Newline: {}\n", "line1\nline2");
    std::cout << std::format("Tab: {}\n", "col1\tcol2");
    std::cout << std::format("Quote: {}\n", "say \"hello\"");
    std::cout << std::format("Brace: {{ and }}\n");
}

// --- 6. Formatting Custom Types ---

struct Point {
    double x, y;
};

template <>
struct std::formatter<Point> : std::formatter<std::string> {
    auto format(const Point& p, auto& ctx) const {
        return std::format_to(ctx.out(), "({:.1f}, {:.1f})", p.x, p.y);
    }
};

void customTypes() {
    std::cout << "\n--- Custom Types ---\n";

    Point p{3.14, 2.71};
    std::cout << std::format("Point: {}\n", p);
}

// --- 7. String View ---

void stringView() {
    std::cout << "\n--- String View ---\n";

    std::string_view sv = "Hello, World!";
    std::cout << std::format("Length: {}\n", sv.length());
    std::cout << std::format("First 5: {:.5}\n", sv);
}

// --- 8. Chrono Formatting ---

void chronoFormatting() {
    std::cout << "\n--- Chrono Formatting ---\n";

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::cout << std::format("Time: {}\n", std::ctime(&time));
}

// --- 9. Format to String ---

void formatToString() {
    std::cout << "\n--- Format to String ---\n";

    std::string result = std::format("{} + {} = {}", 3, 4, 7);
    std::cout << result << "\n";

    // Format to buffer
    char buffer[100];
    std::format_to(buffer, "Buffer: {}", "test");
    std::cout << buffer << "\n";
}

// --- 10. Practical Example ---

void practicalExample() {
    std::cout << "\n--- Practical Example ---\n";

    struct LogEntry {
        std::string level;
        std::string message;
        int code;
    };

    std::vector<LogEntry> logs = {
        {"INFO", "Server started", 0},
        {"WARN", "High memory usage", 1001},
        {"ERROR", "Connection failed", 2002}
    };

    for (const auto& log : logs) {
        std::cout << std::format("[{:<5}] {} (code: {:04d})\n",
            log.level, log.message, log.code);
    }
}

int main() {
    basicFormatting();
    positionArguments();
    formatSpecifiers();
    widthAndAlignment();
    escapeSequences();
    customTypes();
    stringView();
    chronoFormatting();
    formatToString();
    practicalExample();

    std::cout << "\n=== All format scenarios completed ===\n";
    return 0;
}
