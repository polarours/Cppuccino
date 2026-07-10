#include "composite_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Composite Pattern Demo ===\n\n";

    // Create file system structure
    auto root = std::make_shared<composite_pattern::Directory>("root");
    auto src = std::make_shared<composite_pattern::Directory>("src");
    auto docs = std::make_shared<composite_pattern::Directory>("docs");

    src->add(std::make_shared<composite_pattern::File>("main.cpp", 1000));
    src->add(std::make_shared<composite_pattern::File>("utils.cpp", 500));

    docs->add(std::make_shared<composite_pattern::File>("README.md", 200));

    root->add(src);
    root->add(docs);
    root->add(std::make_shared<composite_pattern::File>("CMakeLists.txt", 100));

    // Print structure
    std::cout << "File System Structure:\n";
    root->print();

    std::cout << "\nTotal size: " << root->getSize() << " bytes\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
