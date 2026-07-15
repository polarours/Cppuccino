#include "facade_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== Facade Pattern Demo ===\n\n";

    facade_pattern::ComputerFacade computer;

    std::cout << "Starting computer...\n";
    auto startLog = computer.start();
    for (const auto& step : startLog) {
        std::cout << "  " << step << "\n";
    }

    std::cout << "\nShutting down...\n";
    auto shutLog = computer.shutdown();
    for (const auto& step : shutLog) {
        std::cout << "  " << step << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
