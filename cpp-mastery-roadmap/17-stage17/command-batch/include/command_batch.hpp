#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>

namespace command_batch {

// --- Command Interface ---

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string description() const = 0;
};

// --- Concrete Commands ---

class IncrementCommand : public Command {
public:
    explicit IncrementCommand(int& counter) : counter_(counter) {}

    void execute() override { counter_++; }
    void undo() override { counter_--; }
    std::string description() const override { return "Increment"; }

private:
    int& counter_;
};

class DecrementCommand : public Command {
public:
    explicit DecrementCommand(int& counter) : counter_(counter) {}

    void execute() override { counter_--; }
    void undo() override { counter_++; }
    std::string description() const override { return "Decrement"; }

private:
    int& counter_;
};

// --- Batch Command ---

class BatchCommand : public Command {
public:
    void add(std::shared_ptr<Command> command) {
        commands_.push_back(std::move(command));
    }

    void execute() override {
        std::cout << "[Batch] Executing " << commands_.size() << " commands\n";
        for (auto& cmd : commands_) {
            cmd->execute();
        }
    }

    void undo() override {
        std::cout << "[Batch] Undoing " << commands_.size() << " commands\n";
        for (auto it = commands_.rbegin(); it != commands_.rend(); ++it) {
            (*it)->undo();
        }
    }

    std::string description() const override {
        return "Batch of " + std::to_string(commands_.size()) + " commands";
    }

    std::size_t size() const { return commands_.size(); }

private:
    std::vector<std::shared_ptr<Command>> commands_;
};

} // namespace command_batch
