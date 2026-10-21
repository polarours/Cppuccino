# Command Pattern

## What Is the Command Pattern?

The Command Pattern is a behavioral design pattern that encapsulates a request
as an object. This lets you parameterize a client with different requests, and
supports queuing requests, logging them, undoing operations, and so on.

> **Core idea**: decouple the requester of an action from the executor of it,
> and pass the action around as a command object.

## Why Do We Need the Command Pattern?

### Problem Scenario

Suppose we have a remote control system:

```cpp
// Problem: hard-coded remote control logic
class RemoteControl {
public:
    void pressButton(int button) {
        if (button == 1) {
            light.on();
        } else if (button == 2) {
            light.off();
        } else if (button == 3) {
            fan.on();
        }
        // Every new device means editing this class
    }
};
```

**Problems**:
1. It violates the open-closed principle: adding a device means modifying the remote
2. High coupling: the remote knows every device
3. No undo: no state is kept
4. No history: operations cannot be replayed

### Solution

Use the command pattern:

```cpp
// Abstract command interface
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// Concrete command
class LightOnCommand : public Command {
public:
    explicit LightOnCommand(Light& light) : light_(light) {}

    void execute() override { light_.on(); }
    void undo() override { light_.off(); }

private:
    Light& light_;
};

// The remote control
class RemoteControl {
public:
    void setCommand(int slot, std::unique_ptr<Command> command) {
        commands_[slot] = std::move(command);
    }

    void pressButton(int slot) {
        commands_[slot]->execute();
        history_.push_back(commands_[slot].get());
    }

    void undoLast() {
        if (!history_.empty()) {
            history_.back()->undo();
            history_.pop_back();
        }
    }

private:
    std::map<int, std::unique_ptr<Command>> commands_;
    std::vector<Command*> history_;
};
```

**Benefits**:
1. Decoupling: the remote does not drive the devices directly
2. Extensibility: a new command needs no change to the remote
3. Undo is supported: every command can record history
4. Queuing is supported: commands can be executed in batches
5. Logging is supported: every operation can be recorded

## Pattern Structure

```
                    ┌─────────────┐
                    │  Invoker    │
                    └──────┬──────┘
                           │
              ┌────────────▼────────────┐
              │       Command           │ ← abstract command interface
              │    (execute/undo)       │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  LightOn    │ │  LightOff   │ │   FanOn     │ ← concrete commands
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │       Receiver          │ ← receiver (the device)
              │   (Light, Fan, TV...)   │
              └─────────────────────────┘
```

## Implementation

### 1. Define the Abstract Command

```cpp
#include <memory>
#include <vector>
#include <functional>
#include <iostream>

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};
```

### 2. Implement the Concrete Commands

```cpp
// The devices
class Light {
public:
    void on() { std::cout << "Light is ON\n"; }
    void off() { std::cout << "Light is OFF\n"; }
};

class Fan {
public:
    void on() { std::cout << "Fan is ON\n"; }
    void off() { std::cout << "Fan is OFF\n"; }
};

class TV {
public:
    void on() { std::cout << "TV is ON\n"; }
    void off() { std::cout << "TV is OFF\n"; }
};

// Concrete command: turn the light on
class LightOnCommand : public Command {
public:
    explicit LightOnCommand(Light& light) : light_(light) {}

    void execute() override { light_.on(); }
    void undo() override { light_.off(); }

private:
    Light& light_;
};

// Concrete command: turn the light off
class LightOffCommand : public Command {
public:
    explicit LightOffCommand(Light& light) : light_(light) {}

    void execute() override { light_.off(); }
    void undo() override { light_.on(); }

private:
    Light& light_;
};

// Concrete command: turn the fan on
class FanOnCommand : public Command {
public:
    explicit FanOnCommand(Fan& fan) : fan_(fan) {}

    void execute() override { fan_.on(); }
    void undo() override { fan_.off(); }

private:
    Fan& fan_;
};
```

### 3. Implement a Command Macro

```cpp
// Use a macro to shorten the command definition
#define DECLARE_COMMAND(class_name, receiver_type, on_action, off_action) \
class class_name : public Command { \
public: \
    explicit class_name(receiver_type& receiver) : receiver_(receiver) {} \
    void execute() override { receiver_.on_action(); } \
    void undo() override { receiver_.off_action(); } \
private: \
    receiver_type& receiver_; \
};

// Using the macro
DECLARE_COMMAND(LightOnCmd, Light, on, off)
DECLARE_COMMAND(FanOnCmd, Fan, on, off)
```

### 4. Implement a Command Queue

```cpp
class CommandQueue {
public:
    void enqueue(std::unique_ptr<Command> command) {
        queue_.push_back(std::move(command));
    }

    void executeAll() {
        for (auto& cmd : queue_) {
            cmd->execute();
        }
    }

    void executeAndRecord() {
        for (auto& cmd : queue_) {
            cmd->execute();
            history_.push_back(cmd.get());
        }
    }

private:
    std::vector<std::unique_ptr<Command>> queue_;
    std::vector<Command*> history_;
};
```

### 5. Implement a Macro Command

```cpp
class MacroCommand : public Command {
public:
    void addCommand(std::unique_ptr<Command> command) {
        commands_.push_back(std::move(command));
    }

    void execute() override {
        for (auto& cmd : commands_) {
            cmd->execute();
        }
    }

    void undo() override {
        for (auto it = commands_.rbegin(); it != commands_.rend(); ++it) {
            (*it)->undo();
        }
    }

private:
    std::vector<std::unique_ptr<Command>> commands_;
};
```

### 6. Client Usage

```cpp
int main() {
    Light light;
    Fan fan;

    LightOnCommand lightOn(light);
    LightOffCommand lightOff(light);
    FanOnCommand fanOn(fan);

    // Execute a command
    lightOn.execute();
    lightOn.execute();  // executed more than once
    lightOff.execute();

    // Undo an operation
    lightOn.execute();
    lightOn.undo();

    // Macro command
    MacroCommand partyMode;
    partyMode.addCommand(std::make_unique<LightOnCommand>(light));
    partyMode.addCommand(std::make_unique<FanOnCommand>(fan));
    partyMode.execute();

    return 0;
}
```

## Modern C++ Implementations

### Using std::function

```cpp
class ModernCommand {
public:
    using Action = std::function<void()>;

    ModernCommand(Action execute, Action undo)
        : execute_(std::move(execute)), undo_(std::move(undo)) {}

    void execute() { execute_(); }
    void undo() { undo_(); }

private:
    Action execute_;
    Action undo_;
};

// Using a lambda
ModernCommand lightOnCmd(
    [&light]() { light.on(); },
    [&light]() { light.off(); }
);
```

### Using a Template Command

```cpp
template<typename Receiver, typename Action>
class TemplateCommand : public Command {
public:
    TemplateCommand(Receiver& receiver, Action action)
        : receiver_(receiver), action_(std::move(action)) {}

    void execute() override { action_(receiver_); }
    void undo() override { /* an undo action is needed here */ }

private:
    Receiver& receiver_;
    Action action_;
};

// Using it
TemplateCommand<Light, void(Light&)> lightOnCmd(light,
    [](Light& l) { l.on(); });
```

## Variants and Extensions

### Variant 1: Request Logging

```cpp
class LoggingCommand : public Command {
public:
    LoggingCommand(std::unique_ptr<Command> command, const std::string& name)
        : command_(std::move(command)), name_(name) {}

    void execute() override {
        std::cout << "Executing: " << name_ << "\n";
        command_->execute();
        log_(name_ + " executed at " + currentTime());
    }

    void undo() override {
        command_->undo();
        log_(name_ + " undone at " + currentTime());
    }

private:
    std::unique_ptr<Command> command_;
    std::string name_;
};
```

### Variant 2: Command Replay

```cpp
class ReplayCommand : public Command {
public:
    void addSnapshot(Snapshot snapshot) {
        snapshots_.push_back(snapshot);
    }

    void execute() override {
        for (const auto& snap : snapshots_) {
            restoreState(snap);
        }
    }

private:
    std::vector<Snapshot> snapshots_;
};
```

### Variant 3: Concurrent Commands

```cpp
class ConcurrentCommand : public Command {
public:
    void addCommand(std::unique_ptr<Command> command) {
        commands_.push_back(std::move(command));
    }

    void execute() override {
        std::vector<std::thread> threads;
        for (auto& cmd : commands_) {
            threads.emplace_back([cmd = cmd.get()]() {
                cmd->execute();
            });
        }
        for (auto& t : threads) {
            t.join();
        }
    }

private:
    std::vector<std::unique_ptr<Command>> commands_;
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|------|------|
| **Strategy Pattern** | The strategy pattern encapsulates an algorithm; the command pattern encapsulates a request |
| **Memento Pattern** | The memento saves state; the command pattern performs an action and supports undo |
| **Iterator Pattern** | The iterator walks a collection; the command wraps a single request |

## Best Practices

### 1. Manage Commands with Smart Pointers

```cpp
// Recommended: use unique_ptr
std::unique_ptr<Command> cmd = std::make_unique<LightOnCommand>(light);

// Not recommended: a raw pointer
Command* cmd = new LightOnCommand(light);
```

### 2. Keep Undo Consistent

```cpp
class ConsistentCommand : public Command {
public:
    void execute() override {
        beforeState_ = saveState();
        performAction();
    }

    void undo() override {
        restoreState(beforeState_);
    }

private:
    State beforeState_;
};
```

### 3. Consider Command Idempotency

```cpp
class IdempotentCommand : public Command {
public:
    void execute() override {
        if (!executed_) {
            doExecute();
            executed_ = true;
        }
    }

    void undo() override {
        if (executed_) {
            doUndo();
            executed_ = false;
        }
    }

private:
    bool executed_ = false;
};
```

## Common Traps

### Trap 1: Irreversible Undo

```cpp
// Problem: some operations cannot be undone
class DeleteFileCommand : public Command {
public:
    void execute() override {
        std::remove(filename_.c_str());
    }

    void undo() override {
        // A deleted file cannot be recovered!
    }
};

// Recommended: make sure it can be undone
class SafeDeleteCommand : public Command {
public:
    void execute() override {
        moveToTrash(filename_);  // move to the trash instead of deleting
    }

    void undo() override {
        restoreFromTrash(filename_);
    }
};
```

### Trap 2: Command Object Lifetime

```cpp
// Problem: the command holds a dangling reference
class BadCommand : public Command {
public:
    explicit BadCommand(Light& light) : light_(light) {}  // dangerous!

private:
    Light& light_;  // the reference may dangle
};

// Recommended: use a smart pointer or pass by value
class GoodCommand : public Command {
public:
    explicit GoodCommand(std::shared_ptr<Light> light)
        : light_(std::move(light)) {}

private:
    std::shared_ptr<Light> light_;
};
```

### Trap 3: Memory Leak in a Command Queue

```cpp
// Problem: the command is not released after it runs
class Queue : public Command {
public:
    void execute() override {
        for (auto& cmd : queue_) {
            cmd->execute();
            // cmd is never released, so the memory leaks
        }
    }

private:
    std::vector<Command*> queue_;
};

// Recommended: use unique_ptr so ownership is automatic
class SafeQueue : public Command {
public:
    void execute() override {
        for (auto& cmd : queue_) {
            cmd->execute();
        }
        queue_.clear();  // released automatically
    }

private:
    std::vector<std::unique_ptr<Command>> queue_;
};
```

## Summary

The key points of the command pattern:

- **When to use it**: undo/redo, request queuing, logging, macro commands
- **Core benefits**: decoupling, extensibility, undo support, logging
- **Implementation notes**: use smart pointers, make sure it can be undone, mind
  the lifetime
- **Common traps**: irreversible operations, dangling references, memory leaks

> **Remember**: the essence of the command pattern is "encapsulating the request" —
> turning a request into an object so it can be passed, stored, and manipulated like
> any other object.
