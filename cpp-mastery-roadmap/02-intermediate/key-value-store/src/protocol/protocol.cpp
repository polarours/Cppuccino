#include "protocol.hpp"

#include <algorithm>
#include <cctype>

namespace key_value_store {

namespace {

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string toUpper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return str;
}

} // namespace

// --- ProtocolResponse ---

std::string ProtocolResponse::serialize() const {
    switch (status) {
        case ProtocolStatus::ok:
            return "+OK\n";
        case ProtocolStatus::value:
            return "$" + data + "\n";
        case ProtocolStatus::not_found:
            return "-NOT_FOUND\n";
        case ProtocolStatus::error:
            return "-ERROR " + data + "\n";
        case ProtocolStatus::count:
            return ":" + data + "\n";
        case ProtocolStatus::bye:
            return "+BYE\n";
    }
    return "-ERROR unknown status\n";
}

ProtocolResponse ProtocolResponse::parse(const std::string& line) {
    if (line.empty()) {
        return {ProtocolStatus::error, "empty response"};
    }

    char prefix = line[0];
    std::string content = line.substr(1);

    switch (prefix) {
        case '+':
            if (content == "OK") return {ProtocolStatus::ok, ""};
            if (content == "BYE") return {ProtocolStatus::bye, ""};
            return {ProtocolStatus::ok, content};
        case '$':
            return {ProtocolStatus::value, content};
        case '-':
            if (content.substr(0, 10) == "NOT_FOUND") {
                return {ProtocolStatus::not_found, ""};
            }
            if (content.substr(0, 6) == "ERROR ") {
                return {ProtocolStatus::error, content.substr(6)};
            }
            return {ProtocolStatus::error, content};
        case ':':
            return {ProtocolStatus::count, content};
        default:
            return {ProtocolStatus::error, "invalid response format"};
    }
}

// --- Protocol ---

Protocol::Protocol(KeyValueStore& store) : store_(store) {}

ProtocolResponse Protocol::execute(const std::string& request) {
    std::string trimmed = trim(request);
    if (trimmed.empty()) {
        return {ProtocolStatus::error, "empty command"};
    }

    size_t spacePos = trimmed.find(' ');
    std::string command = toUpper(trimmed.substr(0, spacePos));
    std::string args = (spacePos != std::string::npos)
        ? trim(trimmed.substr(spacePos + 1))
        : "";

    if (command == "SET") return handleSet(args);
    if (command == "GET") return handleGet(args);
    if (command == "DEL" || command == "DELETE") return handleDelete(args);
    if (command == "EXISTS") return handleExists(args);
    if (command == "COUNT") return handleCount();
    if (command == "CLEAR") return handleClear();
    if (command == "PING") return handlePing();
    if (command == "HELP") return handleHelp();

    return handleUnknown(command);
}

ProtocolResponse Protocol::handleSet(const std::string& args) {
    size_t spacePos = args.find(' ');
    if (spacePos == std::string::npos) {
        return {ProtocolStatus::error, "SET requires key and value"};
    }

    std::string key = args.substr(0, spacePos);
    std::string value = trim(args.substr(spacePos + 1));

    if (key.empty() || value.empty()) {
        return {ProtocolStatus::error, "SET requires non-empty key and value"};
    }

    store_.set(key, value);
    return {ProtocolStatus::ok, ""};
}

ProtocolResponse Protocol::handleGet(const std::string& args) {
    std::string key = trim(args);
    if (key.empty()) {
        return {ProtocolStatus::error, "GET requires a key"};
    }

    auto value = store_.get(key);
    if (!value.has_value()) {
        return {ProtocolStatus::not_found, ""};
    }

    return {ProtocolStatus::value, *value};
}

ProtocolResponse Protocol::handleDelete(const std::string& args) {
    std::string key = trim(args);
    if (key.empty()) {
        return {ProtocolStatus::error, "DELETE requires a key"};
    }

    if (store_.erase(key)) {
        return {ProtocolStatus::ok, ""};
    }
    return {ProtocolStatus::not_found, ""};
}

ProtocolResponse Protocol::handleExists(const std::string& args) {
    std::string key = trim(args);
    if (key.empty()) {
        return {ProtocolStatus::error, "EXISTS requires a key"};
    }

    return {ProtocolStatus::ok, store_.exists(key) ? "1" : "0"};
}

ProtocolResponse Protocol::handleCount() {
    return {ProtocolStatus::count, std::to_string(store_.size())};
}

ProtocolResponse Protocol::handleClear() {
    store_.clear();
    return {ProtocolStatus::ok, ""};
}

ProtocolResponse Protocol::handlePing() {
    return {ProtocolStatus::ok, "pong"};
}

ProtocolResponse Protocol::handleHelp() {
    return {ProtocolStatus::value, getHelp()};
}

ProtocolResponse Protocol::handleUnknown(const std::string& command) {
    return {ProtocolStatus::error, "unknown command: " + command};
}

std::string Protocol::getHelp() const {
    return
        "SET key value     - Set a key-value pair\n"
        "GET key           - Get value by key\n"
        "DEL key           - Delete a key\n"
        "EXISTS key        - Check if key exists (returns 1 or 0)\n"
        "COUNT             - Get number of entries\n"
        "CLEAR             - Clear all entries\n"
        "PING              - Ping server\n"
        "HELP              - Show this help\n";
}

} // namespace key_value_store
