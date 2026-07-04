#pragma once

#include "store/key_value_store.hpp"

#include <string>

namespace key_value_store {

enum class CommandAction {
    none,
    load,
    exit
};

struct CommandResult {
    CommandAction action{CommandAction::none};
    bool persist_after_command{false};
    std::string output{};
};

class CommandProcessor {
public:
    explicit CommandProcessor(KeyValueStore& store);

    CommandResult execute(const std::string& line);

private:
    KeyValueStore& store_;
};

} // namespace key_value_store
