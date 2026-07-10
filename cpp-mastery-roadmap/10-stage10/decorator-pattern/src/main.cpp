#include "decorator_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Decorator Pattern Demo ===\n\n";

    // Basic file source
    auto source = std::make_shared<decorator_pattern::FileDataSource>("data.txt");

    // Add encryption
    auto encrypted = std::make_shared<decorator_pattern::EncryptionDecorator>(source);

    // Add compression
    auto compressed = std::make_shared<decorator_pattern::CompressionDecorator>(encrypted);

    // Add logging
    auto logged = std::make_shared<decorator_pattern::LoggingDecorator>(compressed);

    // Read with all decorators
    std::cout << "Reading with all decorators:\n";
    std::string data = logged->readData();
    std::cout << "Final data: " << data << "\n";

    // Write with all decorators
    std::cout << "\nWriting with all decorators:\n";
    logged->writeData("test data");

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
