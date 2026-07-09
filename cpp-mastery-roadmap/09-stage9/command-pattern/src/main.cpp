#include "command_pattern.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== Command Pattern Demo ===\n\n";

    command_pattern::TextEditor editor;
    command_pattern::CommandHistory history;

    // Execute commands
    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, "Hello"));
    history.execute(std::make_shared<command_pattern::InsertCommand>(editor, ", World"));

    std::cout << "Text: " << editor.getText() << "\n";

    // Undo
    history.undo();
    std::cout << "After undo: " << editor.getText() << "\n";

    // Redo
    history.redo();
    std::cout << "After redo: " << editor.getText() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
