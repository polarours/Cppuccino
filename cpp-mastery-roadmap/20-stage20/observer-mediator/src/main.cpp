#include "observer_mediator.hpp"

#include <iostream>

int main() {
    std::cout << "=== Observer Mediator Demo ===\n\n";

    observer_mediator::Mediator mediator;

    mediator.registerHandler("click", [](const observer_mediator::Event& e) {
        std::cout << "Click handler: " << e.data << "\n";
    });

    mediator.notify("button", {"click", "clicked"});

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
