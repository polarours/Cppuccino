#include "command_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_execute() {
    command_pattern::TextEditor editor;
    command_pattern::CommandHistory history;

    auto cmd = std::make_shared<command_pattern::InsertCommand>(editor, "hello");
    history.execute(cmd);

    expect(editor.getText() == "hello", "expected hello");
}

void test_undo() {
    command_pattern::TextEditor editor;
    command_pattern::CommandHistory history;

    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "hello"));
    expect(editor.getText() == "hello", "expected hello");

    history.undo();
    expect(editor.getText() == "", "expected empty after undo");
}

void test_redo() {
    command_pattern::TextEditor editor;
    command_pattern::CommandHistory history;

    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "hello"));
    history.undo();
    expect(editor.getText() == "", "expected empty");

    history.redo();
    expect(editor.getText() == "hello", "expected hello after redo");
}

void test_multiple_commands() {
    command_pattern::TextEditor editor;
    command_pattern::CommandHistory history;

    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "hello"));
    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, " world"));

    expect(editor.getText() == "hello world", "expected hello world");

    history.undo();
    expect(editor.getText() == "hello", "expected hello after undo");

    history.undo();
    expect(editor.getText() == "", "expected empty after undo");
}

void test_undo_stack_size() {
    command_pattern::CommandHistory history;

    expect(history.undoSize() == 0, "expected 0 undo");
    expect(history.redoSize() == 0, "expected 0 redo");

    command_pattern::TextEditor editor;
    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "test"));

    expect(history.undoSize() == 1, "expected 1 undo");
    expect(history.redoSize() == 0, "expected 0 redo");
}

void test_redo_clears_undo() {
    command_pattern::TextEditor editor;
    command_pattern::CommandHistory history;

    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "first"));
    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "second"));
    history.undo();

    // New command should clear redo stack
    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "third"));

    expect(history.redoSize() == 0, "expected redo stack to be cleared");
}

void test_delete_command() {
    command_pattern::TextEditor editor;
    command_pattern::CommandHistory history;

    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "hello"));
    expect(editor.getText() == "hello", "expected hello");

    history.execute(std::make_shared<command_pattern::DeleteCommand>(editor, 3));
    // deleteText removes from the end, so "hello" becomes "he"
    expect(editor.getText() == "he", "expected he after delete");
}

} // namespace

int main() {
    try {
        std::cout << "Command Pattern Tests:\n";
        test_basic_execute();
        std::cout << "  test_basic_execute: PASS\n";

        test_undo();
        std::cout << "  test_undo: PASS\n";

        test_redo();
        std::cout << "  test_redo: PASS\n";

        test_multiple_commands();
        std::cout << "  test_multiple_commands: PASS\n";

        test_undo_stack_size();
        std::cout << "  test_undo_stack_size: PASS\n";

        test_redo_clears_undo();
        std::cout << "  test_redo_clears_undo: PASS\n";

        test_delete_command();
        std::cout << "  test_delete_command: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "command_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\ncommand_pattern_tests passed" << std::endl;
    return 0;
}
