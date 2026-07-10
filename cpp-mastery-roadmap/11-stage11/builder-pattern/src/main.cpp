#include "builder_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Builder Pattern Demo ===\n\n";

    builder_pattern::ComputerDirector director;

    // Build gaming computer
    director.setBuilder(std::make_shared<builder_pattern::GamingComputerBuilder>());
    auto gaming = director.construct();
    std::cout << "Gaming Computer:\n";
    gaming->display();

    // Build office computer
    director.setBuilder(std::make_shared<builder_pattern::OfficeComputerBuilder>());
    auto office = director.construct();
    std::cout << "\nOffice Computer:\n";
    office->display();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
