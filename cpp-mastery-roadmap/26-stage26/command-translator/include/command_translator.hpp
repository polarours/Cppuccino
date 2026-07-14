#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <iostream>

namespace command_translator {

struct RawCommand {
    std::string verb;
    std::string args;
};

struct TranslatedCommand {
    std::string action;
    std::map<std::string, std::string> params;
};

using Translator = std::function<TranslatedCommand(const RawCommand&)>;

class CommandTranslator {
public:
    void registerTranslator(const std::string& verb, Translator translator) {
        translators_[verb] = std::move(translator);
    }

    TranslatedCommand translate(const RawCommand& raw) const {
        auto it = translators_.find(raw.verb);
        if (it != translators_.end()) {
            return it->second(raw);
        }
        return {raw.verb, {{"raw", raw.args}}};
    }

    bool hasTranslator(const std::string& verb) const {
        return translators_.find(verb) != translators_.end();
    }

    std::vector<TranslatedCommand> translateAll(const std::vector<RawCommand>& commands) const {
        std::vector<TranslatedCommand> result;
        result.reserve(commands.size());
        for (const auto& cmd : commands) {
            result.push_back(translate(cmd));
        }
        return result;
    }

private:
    std::map<std::string, Translator> translators_;
};

} // namespace command_translator
