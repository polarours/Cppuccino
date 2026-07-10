#include "null_object_pattern.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <vector>

int main() {
    std::cout << "=== Null Object Pattern Demo ===\n\n";

    std::vector<std::string> types = {"dog", "cat", "bird", "fish"};

    for (const auto& type : types) {
        auto animal = null_object_pattern::AnimalFactory::create(type);
        std::cout << type << ": " << animal->speak() << " - " << animal->move() << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
