#include "mvc_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== MVC Pattern Demo ===\n\n";

    mvc_pattern::TodoController controller;
    controller.addItem("Learn C++");
    controller.addItem("Build project");
    controller.addItem("Write tests");
    controller.toggleItem(1);
    controller.refresh();

    for (const auto& line : controller.output()) {
        std::cout << "  " << line << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
