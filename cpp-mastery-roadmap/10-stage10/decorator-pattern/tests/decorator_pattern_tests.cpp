#include "decorator_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_file_source() {
    auto source = std::make_shared<decorator_pattern::FileDataSource>("test.txt");
    std::string data = source->readData();
    expect(data == "Data from test.txt", "expected file data");
}

void test_encryption_decorator() {
    auto source = std::make_shared<decorator_pattern::FileDataSource>("test.txt");
    auto encrypted = std::make_shared<decorator_pattern::EncryptionDecorator>(source);

    // Should not throw
    std::string data = encrypted->readData();
    expect(!data.empty(), "expected non-empty data");
}

void test_compression_decorator() {
    auto source = std::make_shared<decorator_pattern::FileDataSource>("test.txt");
    auto compressed = std::make_shared<decorator_pattern::CompressionDecorator>(source);

    std::string data = compressed->readData();
    expect(data == "Data from test.txt", "expected decompressed data");
}

void test_logging_decorator() {
    auto source = std::make_shared<decorator_pattern::FileDataSource>("test.txt");
    auto logged = std::make_shared<decorator_pattern::LoggingDecorator>(source);

    std::string data = logged->readData();
    expect(data == "Data from test.txt", "expected logged data");
}

void test_composed_decorators() {
    auto source = std::make_shared<decorator_pattern::FileDataSource>("test.txt");
    auto encrypted = std::make_shared<decorator_pattern::EncryptionDecorator>(source);
    auto compressed = std::make_shared<decorator_pattern::CompressionDecorator>(encrypted);
    auto logged = std::make_shared<decorator_pattern::LoggingDecorator>(compressed);

    // Should not throw
    std::string data = logged->readData();
    expect(!data.empty(), "expected non-empty data");
}

void test_write_through_decorators() {
    auto source = std::make_shared<decorator_pattern::FileDataSource>("test.txt");
    auto encrypted = std::make_shared<decorator_pattern::EncryptionDecorator>(source);

    // Should not throw
    encrypted->writeData("test data");
}

} // namespace

int main() {
    try {
        std::cout << "Decorator Pattern Tests:\n";
        test_basic_file_source();
        std::cout << "  test_basic_file_source: PASS\n";

        test_encryption_decorator();
        std::cout << "  test_encryption_decorator: PASS\n";

        test_compression_decorator();
        std::cout << "  test_compression_decorator: PASS\n";

        test_logging_decorator();
        std::cout << "  test_logging_decorator: PASS\n";

        test_composed_decorators();
        std::cout << "  test_composed_decorators: PASS\n";

        test_write_through_decorators();
        std::cout << "  test_write_through_decorators: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "decorator_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\ndecorator_pattern_tests passed" << std::endl;
    return 0;
}
