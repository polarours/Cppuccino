#include "observer_bus.hpp"

#include <iostream>

int main() {
    std::cout << "=== Observer Bus Demo ===\n\n";

    observer_bus::EventBus bus;
    bus.subscribe("message", [](const observer_bus::Event& e) {
        std::cout << "Received: " << e.data << "\n";
    });

    bus.publish({"message", "Hello!"});

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
