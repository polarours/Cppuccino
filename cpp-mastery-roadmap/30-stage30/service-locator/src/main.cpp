#include "service_locator.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <map>

class Logger {
public:
    std::string log(const std::string& msg) { return "LOG: " + msg; }
};

int main() {
    std::cout << "=== Service Locator Demo ===\n\n";

    service_locator::ServiceLocator locator;
    locator.registerService("logger", std::make_shared<Logger>());

    auto logger = locator.getService<Logger>("logger");
    if (logger) {
        std::cout << logger->log("Hello") << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
