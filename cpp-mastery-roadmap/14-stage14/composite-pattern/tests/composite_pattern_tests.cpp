#include "composite_pattern.hpp"

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

void test_file() {
    auto file = std::make_shared<composite_pattern::File>("test.txt", 100);

    expect(file->getName() == "test.txt", "expected test.txt");
    expect(file->getSize() == 100, "expected 100");
}

void test_directory() {
    auto dir = std::make_shared<composite_pattern::Directory>("root");
    dir->add(std::make_shared<composite_pattern::File>("file1.txt", 100));
    dir->add(std::make_shared<composite_pattern::File>("file2.txt", 200));

    expect(dir->getName() == "root", "expected root");
    expect(dir->getSize() == 300, "expected 300");
    expect(dir->getChildren().size() == 2, "expected 2 children");
}

void test_nested_structure() {
    auto root = std::make_shared<composite_pattern::Directory>("root");
    auto subDir = std::make_shared<composite_pattern::Directory>("subdir");

    subDir->add(std::make_shared<composite_pattern::File>("file1.txt", 100));
    subDir->add(std::make_shared<composite_pattern::File>("file2.txt", 200));

    root->add(subDir);
    root->add(std::make_shared<composite_pattern::File>("root.txt", 50));

    expect(root->getSize() == 350, "expected 350");
}

void test_print_structure() {
    auto root = std::make_shared<composite_pattern::Directory>("root");
    auto subDir = std::make_shared<composite_pattern::Directory>("sub");

    subDir->add(std::make_shared<composite_pattern::File>("file.txt", 100));
    root->add(subDir);
    root->add(std::make_shared<composite_pattern::File>("root.txt", 50));

    // Should not throw
    root->print();
}

} // namespace

int main() {
    try {
        std::cout << "Composite Pattern Tests:\n";
        test_file();
        std::cout << "  test_file: PASS\n";

        test_directory();
        std::cout << "  test_directory: PASS\n";

        test_nested_structure();
        std::cout << "  test_nested_structure: PASS\n";

        test_print_structure();
        std::cout << "  test_print_structure: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "composite_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncomposite_pattern_tests passed" << std::endl;
    return 0;
}
