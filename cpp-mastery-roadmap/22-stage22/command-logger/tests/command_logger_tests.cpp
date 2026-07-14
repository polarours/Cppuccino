#include "command_logger.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_log_entry() {
    command_logger::CommandLogger logger;
    logger.log("ls", "file1.txt", true);

    expect(logger.size() == 1, "expected 1 entry");
    expect(logger.getEntries()[0].command == "ls", "expected ls");
    expect(logger.getEntries()[0].success, "expected success");
}

void test_multiple_entries() {
    command_logger::CommandLogger logger;
    logger.log("cmd1", "result1", true);
    logger.log("cmd2", "result2", false);

    expect(logger.size() == 2, "expected 2 entries");
    expect(!logger.getEntries()[1].success, "expected failure");
}

void test_clear() {
    command_logger::CommandLogger logger;
    logger.log("cmd", "result", true);
    logger.clear();

    expect(logger.size() == 0, "expected 0 after clear");
}

} // namespace

int main() {
    try {
        std::cout << "Command Logger Tests:\n";
        test_log_entry();
        std::cout << "  test_log_entry: PASS\n";

        test_multiple_entries();
        std::cout << "  test_multiple_entries: PASS\n";

        test_clear();
        std::cout << "  test_clear: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_logger_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_logger_tests passed" << std::endl;
    return 0;
}
