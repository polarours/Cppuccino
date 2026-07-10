#include "memento_pattern.hpp"

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

void test_basic_save_restore() {
    memento_pattern::Editor editor;
    memento_pattern::EditorHistory history;

    editor.type("Hello");
    history.saveState(editor.save());

    editor.type(" World");
    expect(editor.getContent() == "Hello World", "expected Hello World");

    auto memento = history.undo();
    editor.restore(memento);
    expect(editor.getContent() == "Hello", "expected Hello");
}

void test_multiple_undos() {
    memento_pattern::Editor editor;
    memento_pattern::EditorHistory history;

    editor.type("A");
    history.saveState(editor.save());

    editor.type("B");
    history.saveState(editor.save());

    editor.type("C");
    expect(editor.getContent() == "ABC", "expected ABC");

    auto memento = history.undo();
    editor.restore(memento);
    expect(editor.getContent() == "AB", "expected AB");

    memento = history.undo();
    editor.restore(memento);
    expect(editor.getContent() == "A", "expected A");
}

void test_history_size() {
    memento_pattern::Editor editor;
    memento_pattern::EditorHistory history;

    expect(history.historySize() == 0, "expected 0");

    history.saveState(editor.save());
    expect(history.historySize() == 1, "expected 1");

    editor.type("test");
    history.saveState(editor.save());
    expect(history.historySize() == 2, "expected 2");
}

void test_undo_empty_history() {
    memento_pattern::EditorHistory history;

    auto memento = history.undo();
    expect(memento == nullptr, "expected nullptr for empty history");
}

} // namespace

int main() {
    try {
        std::cout << "Memento Pattern Tests:\n";
        test_basic_save_restore();
        std::cout << "  test_basic_save_restore: PASS\n";

        test_multiple_undos();
        std::cout << "  test_multiple_undos: PASS\n";

        test_history_size();
        std::cout << "  test_history_size: PASS\n";

        test_undo_empty_history();
        std::cout << "  test_undo_empty_history: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "memento_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nmemento_pattern_tests passed" << std::endl;
    return 0;
}
