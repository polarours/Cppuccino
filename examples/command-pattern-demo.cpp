// examples/command-pattern-demo.cpp
// Demonstrates Command Pattern with TextEditor, InsertCommand, CommandHistory (undo/redo).
// Compile: g++ -std=c++20 -o command-pattern-demo command-pattern-demo.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace command_pattern {

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class TextEditor {
public:
    void insertText(const std::string& s) { text_ += s; }
    void deleteText(std::size_t n) { if (n > text_.size()) n = text_.size(); text_.resize(text_.size() - n); }
    std::string getText() const { return text_; }
private:
    std::string text_;
};

class InsertCommand : public Command {
public:
    InsertCommand(TextEditor& e, std::string s) : editor_(e), text_(std::move(s)) {}
    void execute() override { editor_.insertText(text_); }
    void undo() override { editor_.deleteText(text_.size()); }
private:
    TextEditor& editor_;
    std::string text_;
};

class DeleteCommand : public Command {
public:
    explicit DeleteCommand(TextEditor& e, std::size_t n) : editor_(e), len_(n) {}
    void execute() override { editor_.deleteText(len_); }
    void undo() override { /* cannot undo delete without saved state */ }
private:
    TextEditor& editor_;
    std::size_t len_;
};

class CommandHistory {
public:
    void execute(std::shared_ptr<Command> cmd) {
        cmd->execute();
        undoStack_.push_back(std::move(cmd));
        redoStack_.clear();
    }
    void undo() {
        if (undoStack_.empty()) return;
        auto cmd = std::move(undoStack_.back()); undoStack_.pop_back();
        cmd->undo();
        redoStack_.push_back(std::move(cmd));
    }
    void redo() {
        if (redoStack_.empty()) return;
        auto cmd = std::move(redoStack_.back()); redoStack_.pop_back();
        cmd->execute();
        undoStack_.push_back(std::move(cmd));
    }
    std::size_t undoSize() const { return undoStack_.size(); }
    std::size_t redoSize() const { return redoStack_.size(); }
private:
    std::vector<std::shared_ptr<Command>> undoStack_;
    std::vector<std::shared_ptr<Command>> redoStack_;
};

} // namespace command_pattern

int main() {
    using namespace command_pattern;
    std::cout << "=== Command Pattern Demo ===\n\n";

    TextEditor editor;
    CommandHistory history;

    history.execute(std::make_shared<InsertCommand>(editor, "Hello "));
    history.execute(std::make_shared<InsertCommand>(editor, "World"));
    std::cout << "After insert: \"" << editor.getText() << "\"\n";
    std::cout << "Undo stack size: " << history.undoSize() << "\n";

    history.undo();
    std::cout << "After undo: \"" << editor.getText() << "\"\n";

    history.redo();
    std::cout << "After redo: \"" << editor.getText() << "\"\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
