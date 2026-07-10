#include "memento_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Memento Pattern Demo ===\n\n";

    memento_pattern::Editor editor;
    memento_pattern::EditorHistory history;

    // Type some text
    editor.type("Hello");
    history.saveState(editor.save());

    editor.type(" World");
    history.saveState(editor.save());

    editor.type("!");
    std::cout << "Current: " << editor.getContent() << "\n";

    // Undo
    auto memento = history.undo();
    editor.restore(memento);
    std::cout << "After undo: " << editor.getContent() << "\n";

    memento = history.undo();
    editor.restore(memento);
    std::cout << "After undo: " << editor.getContent() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
