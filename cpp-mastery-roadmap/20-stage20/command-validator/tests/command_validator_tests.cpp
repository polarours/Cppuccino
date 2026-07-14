#include "command_validator.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_valid_command() {
    command_validator::CommandValidator validator;
    validator.addValidator([](const std::string& cmd) {
        return !cmd.empty();
    });

    expect(validator.validate("ls -la"), "expected valid command");
}

void test_invalid_command() {
    command_validator::CommandValidator validator;
    validator.addValidator([](const std::string& cmd) {
        return !cmd.empty();
    });

    expect(!validator.validate(""), "expected invalid empty command");
}

void test_multiple_validators() {
    command_validator::CommandValidator validator;
    validator.addValidator([](const std::string& cmd) {
        return !cmd.empty();
    });
    validator.addValidator([](const std::string& cmd) {
        return cmd.size() <= 100;
    });

    expect(validator.validate("ls"), "expected valid");
    expect(!validator.validate(""), "expected invalid");
}

void test_get_errors() {
    command_validator::CommandValidator validator;

    auto errors = validator.getErrors("");
    expect(errors.size() == 1, "expected 1 error");

    auto longCmd = std::string(150, 'a');
    errors = validator.getErrors(longCmd);
    expect(errors.size() == 1, "expected 1 error for long command");
}

} // namespace

int main() {
    try {
        std::cout << "Command Validator Tests:\n";
        test_valid_command();
        std::cout << "  test_valid_command: PASS\n";

        test_invalid_command();
        std::cout << "  test_invalid_command: PASS\n";

        test_multiple_validators();
        std::cout << "  test_multiple_validators: PASS\n";

        test_get_errors();
        std::cout << "  test_get_errors: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_validator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_validator_tests passed" << std::endl;
    return 0;
}
