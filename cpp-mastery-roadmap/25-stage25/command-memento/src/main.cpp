#include "command_memento.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Command Memento Demo ===\n\n";

    command_memento::Editor editor;
    command_memento::History history;

    editor.type("Hello");
    history.push(editor.save());
    editor.type(" World");
    std::cout << "Current: " << editor.content() << "\n";

    editor.restore(history.pop());
    std::cout << "After restore: " << editor.content() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
