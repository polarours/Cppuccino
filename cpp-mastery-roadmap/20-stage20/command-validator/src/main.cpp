#include "command_validator.hpp"

#include <iostream>

int main() {
    std::cout << "=== Command Validator Demo ===\n\n";

    command_validator::CommandValidator validator;
    validator.addValidator([](const std::string& cmd) {
        return !cmd.empty();
    });

    std::cout << "Validate 'ls -la': " << std::boolalpha << validator.validate("ls -la") << "\n";
    std::cout << "Validate '': " << validator.validate("") << "\n";

    auto errors = validator.getErrors("");
    std::cout << "Errors for empty: " << errors.size() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
