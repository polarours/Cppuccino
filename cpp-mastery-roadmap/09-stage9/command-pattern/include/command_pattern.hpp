#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <iostream>

namespace command_pattern {

// --- Command Interface ---

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// --- Concrete Commands ---

class TextEditor {
public:
    void insertText(const std::string& text) {
        text_ += text;
        std::cout << "Inserted: " << text << "\n";
    }

    void deleteText(std::size_t length) {
        if (length > text_.size()) {
            length = text_.size();
        }
        text_ = text_.substr(0, text_.size() - length);
        std::cout << "Deleted " << length << " characters\n";
    }

    std::string getText() const { return text_; }

private:
    std::string text_;
};

class InsertCommand : public Command {
public:
    InsertCommand(TextEditor& editor, std::string text)
        : editor_(editor), text_(std::move(text)) {}

    void execute() override {
        editor_.insertText(text_);
    }

    void undo() override {
        editor_.deleteText(text_.size());
    }

private:
    TextEditor& editor_;
    std::string text_;
};

class DeleteCommand : public Command {
public:
    DeleteCommand(TextEditor& editor, std::size_t length)
        : editor_(editor), length_(length) {}

    void execute() override {
        editor_.deleteText(length_);
    }

    void undo() override {
        std::cout << "Cannot undo delete (text not saved)\n";
    }

private:
    TextEditor& editor_;
    std::size_t length_;
};

// --- Invoker ---

class CommandHistory {
public:
    void execute(std::shared_ptr<Command> command) {
        command->execute();
        undoStack_.push_back(std::move(command));
        redoStack_.clear();
    }

    void undo() {
        if (undoStack_.empty()) {
            std::cout << "Nothing to undo\n";
            return;
        }

        auto command = std::move(undoStack_.back());
        undoStack_.pop_back();

        command->undo();
        redoStack_.push_back(std::move(command));
    }

    void redo() {
        if (redoStack_.empty()) {
            std::cout << "Nothing to redo\n";
            return;
        }

        auto command = std::move(redoStack_.back());
        redoStack_.pop_back();

        command->execute();
        undoStack_.push_back(std::move(command));
    }

    std::size_t undoSize() const { return undoStack_.size(); }
    std::size_t redoSize() const { return redoStack_.size(); }

private:
    std::vector<std::shared_ptr<Command>> undoStack_;
    std::vector<std::shared_ptr<Command>> redoStack_;
};

} // namespace command_pattern
