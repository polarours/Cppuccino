#include "command_translator.hpp"

#include <iostream>

int main() {
    std::cout << "=== Command Translator Demo ===\n\n";

    command_translator::CommandTranslator translator;
    translator.registerTranslator("mv", [](const command_translator::RawCommand& raw) {
        return command_translator::TranslatedCommand{"rename", {{"source", raw.args}}};
    });

    auto result = translator.translate({"mv", "file.txt"});
    std::cout << "Action: " << result.action << "\n";
    std::cout << "Source: " << result.params.at("source") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
