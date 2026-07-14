#include "command_snapshot.hpp"

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

void test_append_and_undo() {
    command_snapshot::TextState state;
    command_snapshot::CommandHistory history;

    history.execute(std::make_shared<command_snapshot::AppendCommand>("Hello"), state);
    expect(state.text() == "Hello", "expected Hello");

    history.undo(state);
    expect(state.text() == "", "expected empty after undo");
}

void test_redo() {
    command_snapshot::TextState state;
    command_snapshot::CommandHistory history;

    history.execute(std::make_shared<command_snapshot::AppendCommand>("Hello"), state);
    history.undo(state);
    expect(state.text() == "", "expected empty");

    history.redo(state);
    expect(state.text() == "Hello", "expected Hello after redo");
}

void test_new_command_clears_redo() {
    command_snapshot::TextState state;
    command_snapshot::CommandHistory history;

    history.execute(std::make_shared<command_snapshot::AppendCommand>("Hello"), state);
    history.undo(state);
    history.execute(std::make_shared<command_snapshot::AppendCommand>("World"), state);

    expect(history.redoSize() == 0, "expected redo stack cleared");
    expect(state.text() == "World", "expected World");
}

void test_stack_sizes() {
    command_snapshot::TextState state;
    command_snapshot::CommandHistory history;

    history.execute(std::make_shared<command_snapshot::AppendCommand>("A"), state);
    history.execute(std::make_shared<command_snapshot::AppendCommand>("B"), state);
    expect(history.undoSize() == 2, "expected 2 undo");
    expect(history.redoSize() == 0, "expected 0 redo");

    history.undo(state);
    expect(history.undoSize() == 1, "expected 1 undo");
    expect(history.redoSize() == 1, "expected 1 redo");
}

} // namespace

int main() {
    try {
        std::cout << "Command Snapshot Tests:\n";
        test_append_and_undo();
        std::cout << "  test_append_and_undo: PASS\n";

        test_redo();
        std::cout << "  test_redo: PASS\n";

        test_new_command_clears_redo();
        std::cout << "  test_new_command_clears_redo: PASS\n";

        test_stack_sizes();
        std::cout << "  test_stack_sizes: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_snapshot_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_snapshot_tests passed" << std::endl;
    return 0;
}
