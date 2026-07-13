#include "observer_filter.hpp"

#include <iostream>

int main() {
    std::cout << "=== Observer Filter Demo ===\n\n";

    observer_filter::FilteredDispatcher dispatcher;

    dispatcher.onEvent([](const observer_filter::Event& e) {
        std::cout << "Event: " << e.data << " (priority: " << e.priority << ")\n";
    });

    dispatcher.addFilter([](const observer_filter::Event& e) {
        return e.priority >= 5;
    });

    dispatcher.emit({"click", "low priority", 3});
    dispatcher.emit({"click", "high priority", 10});

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
