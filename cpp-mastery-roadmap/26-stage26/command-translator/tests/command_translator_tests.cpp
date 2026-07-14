#include "command_translator.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_translate() {
    command_translator::CommandTranslator translator;
    translator.registerTranslator("mv", [](const command_translator::RawCommand& raw) {
        return command_translator::TranslatedCommand{"rename", {{"source", raw.args}}};
    });

    auto result = translator.translate({"mv", "file.txt"});
    expect(result.action == "rename", "expected rename");
    expect(result.params.at("source") == "file.txt", "expected file.txt");
}

void test_unknown_passthrough() {
    command_translator::CommandTranslator translator;

    auto result = translator.translate({"ls", "-la"});
    expect(result.action == "ls", "expected ls passthrough");
    expect(result.params.at("raw") == "-la", "expected raw args");
}

void test_translate_all() {
    command_translator::CommandTranslator translator;
    translator.registerTranslator("a", [](const command_translator::RawCommand&) {
        return command_translator::TranslatedCommand{"A", {}};
    });

    std::vector<command_translator::RawCommand> commands = {{"a", ""}, {"b", "x"}};
    auto results = translator.translateAll(commands);
    expect(results.size() == 2, "expected 2 results");
    expect(results[0].action == "A", "expected A");
    expect(results[1].action == "b", "expected b passthrough");
}

} // namespace

int main() {
    try {
        std::cout << "Command Translator Tests:\n";
        test_basic_translate();
        std::cout << "  test_basic_translate: PASS\n";

        test_unknown_passthrough();
        std::cout << "  test_unknown_passthrough: PASS\n";

        test_translate_all();
        std::cout << "  test_translate_all: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_translator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_translator_tests passed" << std::endl;
    return 0;
}
