#include "observer_filter_chain.hpp"

#include <iostream>

int main() {
    std::cout << "=== Observer Filter Chain Demo ===\n\n";

    observer_filter_chain::FilterChain chain;
    chain.addFilter([](const observer_filter_chain::Event& e) {
        return observer_filter_chain::Event{e.type, "[" + e.type + "] " + e.data};
    });

    observer_filter_chain::FilteredDispatcher dispatcher(chain);
    dispatcher.setHandler([](const observer_filter_chain::Event& e) {
        std::cout << "Dispatched: " << e.data << "\n";
    });

    dispatcher.emit({"info", "system ready"});

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
