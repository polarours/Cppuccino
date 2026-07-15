#include "delegation.hpp"

#include <iostream>

int main() {
    std::cout << "=== Delegation Pattern Demo ===\n\n";

    delegation_pattern::Button btn(std::make_shared<delegation_pattern::TextRenderer>("Click"), "OK");
    std::cout << "Button: " << btn.draw() << "\n";

    btn.setRenderer(std::make_shared<delegation_pattern::TextRenderer>("Submit"));
    std::cout << "Updated: " << btn.draw() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
