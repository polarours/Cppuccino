#include "singleton_pattern.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== Singleton Pattern Demo ===\n\n";

    // Logger singleton
    auto& logger = singleton_pattern::Logger::getInstance();
    logger.log("Application started");
    logger.log("Processing data");
    logger.log("Application finished");

    std::cout << "Total logs: " << logger.getLogCount() << "\n\n";

    // Config singleton
    auto& config = singleton_pattern::Config::getInstance();
    config.set("host", "localhost");
    config.set("port", "8080");

    std::cout << "Config host: " << config.get("host") << "\n";
    std::cout << "Config port: " << config.get("port") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
