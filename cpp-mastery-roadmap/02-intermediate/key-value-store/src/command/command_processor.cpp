#include "command_processor.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>

namespace key_value_store {

namespace {

std::string trim_leading_spaces(std::string text) {
    const auto first_non_space = std::find_if(text.begin(), text.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    text.erase(text.begin(), first_non_space);
    return text;
}

std::string upper_copy(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return text;
}

CommandResult bad_usage(const std::string& usage) {
    return CommandResult{CommandAction::none, false, "ERROR " + usage};
}

bool has_extra_arguments(std::istringstream& stream) {
    std::string extra;
    return static_cast<bool>(stream >> extra);
}

} // namespace

CommandProcessor::CommandProcessor(KeyValueStore& store)
    : store_(store) {}

CommandResult CommandProcessor::execute(const std::string& line) {
    std::istringstream stream(line);
    std::string command;

    if (!(stream >> command)) {
        return CommandResult{CommandAction::none, false, "ERROR empty command"};
    }

    command = upper_copy(command);

    if (command == "SET") {
        std::string key;
        if (!(stream >> key)) {
            return bad_usage("usage: SET <key> <value>");
        }

        std::string value;
        std::getline(stream, value);
        value = trim_leading_spaces(value);
        if (value.empty()) {
            return bad_usage("usage: SET <key> <value>");
        }

        store_.set(key, value);
        return CommandResult{CommandAction::none, true, "OK"};
    }

    if (command == "GET") {
        std::string key;
        if (!(stream >> key) || has_extra_arguments(stream)) {
            return bad_usage("usage: GET <key>");
        }

        const auto value = store_.get(key);
        if (!value.has_value()) {
            return CommandResult{CommandAction::none, false, "NOT_FOUND"};
        }

        return CommandResult{CommandAction::none, false, "VALUE " + *value};
    }

    if (command == "DELETE") {
        std::string key;
        if (!(stream >> key) || has_extra_arguments(stream)) {
            return bad_usage("usage: DELETE <key>");
        }

        if (!store_.erase(key)) {
            return CommandResult{CommandAction::none, false, "NOT_FOUND"};
        }

        return CommandResult{CommandAction::none, true, "DELETED"};
    }

    if (command == "EXISTS") {
        std::string key;
        if (!(stream >> key) || has_extra_arguments(stream)) {
            return bad_usage("usage: EXISTS <key>");
        }

        return CommandResult{
            CommandAction::none,
            false,
            store_.exists(key) ? "TRUE" : "FALSE"
        };
    }

    if (command == "COUNT") {
        if (has_extra_arguments(stream)) {
            return bad_usage("usage: COUNT");
        }

        return CommandResult{
            CommandAction::none,
            false,
            "COUNT " + std::to_string(store_.size())
        };
    }

    if (command == "CLEAR") {
        if (has_extra_arguments(stream)) {
            return bad_usage("usage: CLEAR");
        }

        store_.clear();
        return CommandResult{CommandAction::none, true, "CLEARED"};
    }

    if (command == "SAVE") {
        if (has_extra_arguments(stream)) {
            return bad_usage("usage: SAVE");
        }

        return CommandResult{CommandAction::none, true, "SAVED"};
    }

    if (command == "LOAD") {
        if (has_extra_arguments(stream)) {
            return bad_usage("usage: LOAD");
        }

        return CommandResult{CommandAction::load, false, "LOADED"};
    }

    if (command == "EXIT" || command == "QUIT") {
        if (has_extra_arguments(stream)) {
            return bad_usage("usage: EXIT");
        }

        return CommandResult{CommandAction::exit, false, "BYE"};
    }

    return CommandResult{CommandAction::none, false, "ERROR unknown command"};
}

} // namespace key_value_store
