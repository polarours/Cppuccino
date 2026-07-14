#include "command_memento.hpp"

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

void test_save_and_restore() {
    command_memento::Editor editor;
    command_memento::History history;

    editor.type("Hello");
    history.push(editor.save());

    editor.type(" World");
    expect(editor.content() == "Hello World", "expected Hello World");

    editor.restore(history.pop());
    expect(editor.content() == "Hello", "expected Hello after restore");
}

void test_multiple_save_restore() {
    command_memento::Editor editor;
    command_memento::History history;

    history.push(editor.save());
    editor.type("A");
    history.push(editor.save());
    editor.type("B");
    history.push(editor.save());
    editor.type("C");

    expect(editor.content() == "ABC", "expected ABC");

    editor.restore(history.pop());
    expect(editor.content() == "AB", "expected AB");

    editor.restore(history.pop());
    expect(editor.content() == "A", "expected A");

    editor.restore(history.pop());
    expect(editor.content() == "", "expected empty");
}

void test_remove() {
    command_memento::Editor editor;
    command_memento::History history;

    editor.type("Hello");
    history.push(editor.save());

    editor.remove(3);
    expect(editor.content() == "He", "expected He");

    editor.restore(history.pop());
    expect(editor.content() == "Hello", "expected Hello");
}

} // namespace

int main() {
    try {
        std::cout << "Command Memento Tests:\n";
        test_save_and_restore();
        std::cout << "  test_save_and_restore: PASS\n";

        test_multiple_save_restore();
        std::cout << "  test_multiple_save_restore: PASS\n";

        test_remove();
        std::cout << "  test_remove: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_memento_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_memento_tests passed" << std::endl;
    return 0;
}
