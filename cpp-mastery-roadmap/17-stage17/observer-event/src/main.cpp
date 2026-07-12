#include "observer_event.hpp"

#include <iostream>

int main() {
    std::cout << "=== Observer Event Demo ===\n\n";

    observer_event::EventDispatcher dispatcher;

    dispatcher.on("click", [](const observer_event::Event& e) {
        std::cout << "Click handler: " << e.data << "\n";
    });

    dispatcher.on("hover", [](const observer_event::Event& e) {
        std::cout << "Hover handler: " << e.data << "\n";
    });

    dispatcher.emit({"click", "button", "clicked"});
    dispatcher.emit({"hover", "button", "hovered"});

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
