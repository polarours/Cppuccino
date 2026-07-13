#include "command_history.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_add_command() {
    command_history::HistoryManager history;

    history.add("ls -la", "file1.txt file2.txt");
    expect(history.size() == 1, "expected 1 entry");

    auto last = history.getLast();
    expect(last.command == "ls -la", "expected ls -la");
    expect(last.result == "file1.txt file2.txt", "expected result");
}

void test_multiple_commands() {
    command_history::HistoryManager history;

    history.add("cmd1");
    history.add("cmd2");
    history.add("cmd3");

    expect(history.size() == 3, "expected 3 entries");
}

void test_undo() {
    command_history::HistoryManager history;

    history.add("cmd1");
    history.add("cmd2");
    expect(history.size() == 2, "expected 2 entries");

    history.undo();
    expect(history.size() == 1, "expected 1 entry after undo");

    auto last = history.getLast();
    expect(last.command == "cmd1", "expected cmd1");
}

void test_max_size() {
    command_history::HistoryManager history(3);

    history.add("cmd1");
    history.add("cmd2");
    history.add("cmd3");
    history.add("cmd4");

    expect(history.size() == 3, "expected max 3 entries");
}

void test_clear() {
    command_history::HistoryManager history;

    history.add("cmd1");
    history.add("cmd2");
    history.clear();

    expect(history.size() == 0, "expected 0 after clear");
}

} // namespace

int main() {
    try {
        std::cout << "Command History Tests:\n";
        test_add_command();
        std::cout << "  test_add_command: PASS\n";

        test_multiple_commands();
        std::cout << "  test_multiple_commands: PASS\n";

        test_undo();
        std::cout << "  test_undo: PASS\n";

        test_max_size();
        std::cout << "  test_max_size: PASS\n";

        test_clear();
        std::cout << "  test_clear: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_history_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_history_tests passed" << std::endl;
    return 0;
}
