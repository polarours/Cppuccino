#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace command_validator {

// --- Validator ---

using Validator = std::function<bool(const std::string&)>;

// --- Command Validator ---

class CommandValidator {
public:
    void addValidator(Validator validator) {
        validators_.push_back(std::move(validator));
    }

    bool validate(const std::string& command) const {
        for (const auto& validator : validators_) {
            if (!validator(command)) {
                return false;
            }
        }
        return true;
    }

    std::vector<std::string> getErrors(const std::string& command) const {
        std::vector<std::string> errors;
        // Simplified: just check basic rules
        if (command.empty()) {
            errors.push_back("Command cannot be empty");
        }
        if (command.size() > 100) {
            errors.push_back("Command too long");
        }
        return errors;
    }

private:
    std::vector<Validator> validators_;
};

} // namespace command_validator
