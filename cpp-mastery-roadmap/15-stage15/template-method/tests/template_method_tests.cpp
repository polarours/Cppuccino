#include "template_method.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_csv_miner() {
    template_method::CSVDataMiner miner;
    // Should not throw
    miner.mine("data.csv");
}

void test_json_miner() {
    template_method::JSONDataMiner miner;
    // Should not throw
    miner.mine("data.json");
}

void test_xml_miner() {
    template_method::XMLDataMiner miner;
    // Should not throw
    miner.mine("data.xml");
}

void test_hook_override() {
    template_method::JSONDataMiner miner;
    // JSON miner overrides openFile hook
    miner.mine("test.json");
}

} // namespace

int main() {
    try {
        std::cout << "Template Method Tests:\n";
        test_csv_miner();
        std::cout << "  test_csv_miner: PASS\n";

        test_json_miner();
        std::cout << "  test_json_miner: PASS\n";

        test_xml_miner();
        std::cout << "  test_xml_miner: PASS\n";

        test_hook_override();
        std::cout << "  test_hook_override: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "template_method_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ntemplate_method_tests passed" << std::endl;
    return 0;
}
