#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

namespace command_snapshot {

class State {
public:
    virtual ~State() = default;
    virtual std::shared_ptr<State> clone() const = 0;
    virtual std::string describe() const = 0;
};

class TextState : public State {
public:
    explicit TextState(std::string text = "") : text_(std::move(text)) {}

    std::shared_ptr<State> clone() const override {
        return std::make_shared<TextState>(text_);
    }

    std::string describe() const override { return text_; }

    const std::string& text() const { return text_; }
    void setText(std::string t) { text_ = std::move(t); }

private:
    std::string text_;
};

class Command {
public:
    virtual ~Command() = default;
    virtual void execute(State& state) = 0;
    virtual void undo(State& state) = 0;
    virtual std::string description() const = 0;
};

class AppendCommand : public Command {
public:
    explicit AppendCommand(std::string text) : text_(std::move(text)) {}

    void execute(State& state) override {
        auto& ts = dynamic_cast<TextState&>(state);
        ts.setText(ts.text() + text_);
    }

    void undo(State& state) override {
        auto& ts = dynamic_cast<TextState&>(state);
        auto t = ts.text();
        ts.setText(t.substr(0, t.size() - text_.size()));
    }

    std::string description() const override { return "Append(\"" + text_ + "\")"; }

private:
    std::string text_;
};

class CommandHistory {
public:
    void execute(std::shared_ptr<Command> cmd, State& state) {
        cmd->execute(state);
        undoStack_.push_back(cmd);
        redoStack_.clear();
    }

    bool undo(State& state) {
        if (undoStack_.empty()) return false;
        auto cmd = undoStack_.back();
        undoStack_.pop_back();
        cmd->undo(state);
        redoStack_.push_back(cmd);
        return true;
    }

    bool redo(State& state) {
        if (redoStack_.empty()) return false;
        auto cmd = redoStack_.back();
        redoStack_.pop_back();
        cmd->execute(state);
        undoStack_.push_back(cmd);
        return true;
    }

    std::size_t undoSize() const { return undoStack_.size(); }
    std::size_t redoSize() const { return redoStack_.size(); }
    void clear() { undoStack_.clear(); redoStack_.clear(); }

private:
    std::vector<std::shared_ptr<Command>> undoStack_;
    std::vector<std::shared_ptr<Command>> redoStack_;
};

} // namespace command_snapshot
