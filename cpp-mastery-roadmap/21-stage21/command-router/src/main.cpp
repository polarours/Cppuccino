#include "command_router.hpp"

#include <iostream>

int main() {
    std::cout << "=== Command Router Demo ===\n\n";

    command_router::CommandRouter router;
    router.addRoute("hello", [](const std::string&) { return "Hello!"; });
    router.addRoute("bye", [](const std::string&) { return "Goodbye!"; });

    std::cout << router.route("hello") << "\n";
    std::cout << router.route("bye") << "\n";
    std::cout << router.route("unknown") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
