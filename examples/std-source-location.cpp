#include <iostream>
#include <string>
#include <sstream>
#include <source_location>

// std::source_location - C++20 location tracking
// Demonstrates how to capture file, line, and function information
// at compile time for better diagnostics and logging.

[[nodiscard]] std::string getLocation(
    const std::source_location& loc = std::source_location::current()) {
    std::ostringstream oss;
    oss << loc.file_name() << ":" << loc.line()
        << " in " << loc.function_name();
    return oss.str();
}

void report(const std::string& message,
            const std::source_location& loc = std::source_location::current()) {
    std::cout << "[" << loc.file_name() << ":" << loc.line() << "] "
              << message << "\n";
}

void nestedFunction() {
    report("Inside nested function");
}

class Demo {
public:
    void method(const std::source_location& loc = std::source_location::current()) {
        std::cout << "Method called from: "
                  << loc.file_name() << ":" << loc.line() << "\n";
    }
};

int main() {
    std::cout << "=== std::source_location Demo (C++20) ===\n\n";

    // Get location of call site
    std::cout << "Current location: " << getLocation() << "\n";

    // Report function
    report("This message has automatic source location");

    // Nested call
    nestedFunction();

    // Class method with implicit source_location
    Demo demo;
    demo.method();

    // Conditional diagnostics
    bool condition = false;
    if (!condition) {
        std::cerr << "Error at " << getLocation() << "\n";
    }

    return 0;
}
