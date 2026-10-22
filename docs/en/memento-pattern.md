# Memento Pattern

## What Is the Memento Pattern?

The Memento Pattern is a behavioral design pattern that allows you to save and
restore the previous state of an object without exposing its implementation
details.

> **Core idea**: capture the internal state of an object so it can be restored
> later, while keeping the encapsulation intact.

## Why Do We Need the Memento Pattern?

### Problem Scenario

Suppose we have a text editor that needs to support undo:

```cpp
// Problem: how do we save and restore the editor state?
class TextEditor {
public:
    void setTypeface(const std::string& typeface) {
        typeface_ = typeface;
    }

    void setSize(int size) {
        size_ = size;
    }

    void insertText(const std::string& text) {
        content_ += text;
    }

    // How do we save the state?
    // How do we restore the state?

private:
    std::string typeface_;
    int size_ = 12;
    std::string content_;
};
```

**Problems**:
1. Several history states have to be kept
2. Exposing the internal state directly breaks encapsulation
3. Undo/redo is hard to implement

### Solution

Use the memento pattern:

```cpp
// Memento: holds the state
class EditorMemento {
public:
    EditorMemento(const std::string& typeface, int size, const std::string& content)
        : typeface_(typeface), size_(size), content_(content) {}

    std::string getTypeface() const { return typeface_; }
    int getSize() const { return size_; }
    std::string getContent() const { return content_; }

private:
    std::string typeface_;
    int size_;
    std::string content_;
};

// Originator: creates and restores mementos
class TextEditor {
public:
    std::unique_ptr<EditorMemento> save() const {
        return std::make_unique<EditorMemento>(typeface_, size_, content_);
    }

    void restore(const EditorMemento& memento) {
        typeface_ = memento.getTypeface();
        size_ = memento.getSize();
        content_ = memento.getContent();
    }

    void setTypeface(const std::string& typeface) { typeface_ = typeface; }
    void setSize(int size) { size_ = size; }
    void insertText(const std::string& text) { content_ += text; }

private:
    std::string typeface_ = "Arial";
    int size_ = 12;
    std::string content_;
};

// Caretaker: manages the mementos
class Caretaker {
public:
    void addMemento(std::unique_ptr<EditorMemento> memento) {
        memos_.push_back(std::move(memento));
    }

    EditorMemento& getMemento(size_t index) {
        return *memos_[index];
    }

    size_t size() const { return memos_.size(); }

private:
    std::vector<std::unique_ptr<EditorMemento>> memos_;
};
```

**Benefits**:
1. Encapsulation is preserved: the internal state is never exposed
2. Undo is supported: several states can be kept
3. The originator stays simple: it does not need to know how to save

## Pattern Structure

```
                    ┌─────────────┐
                    │  Caretaker  │
                    └──────┬──────┘
                           │ manages
                           │
              ┌────────────▼────────────┐
              │     Memento             │ ← memento
              │   (EditorMemento)       │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │      Originator         │ ← originator
              │     (TextEditor)        │
              └─────────────────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← client
                    └─────────────────┘
```

## Implementation

### 1. Define the Memento

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <stack>

class GameMemento {
public:
    GameMemento(int health, int level, const std::string& position)
        : health_(health), level_(level), position_(position) {}

    int getHealth() const { return health_; }
    int getLevel() const { return level_; }
    std::string getPosition() const { return position_; }

private:
    int health_;
    int level_;
    std::string position_;
};
```

### 2. Define the Originator

```cpp
class Game {
public:
    Game(const std::string& name) : name_(name), health_(100), level_(1), position_("start") {}

    std::unique_ptr<GameMemento> save() const {
        return std::make_unique<GameMemento>(health_, level_, position_);
    }

    void restore(const GameMemento& memento) {
        health_ = memento.getHealth();
        level_ = memento.getLevel();
        position_ = memento.getPosition();
    }

    void move(const std::string& newPos) {
        position_ = newPos;
        std::cout << name_ << " moved to " << position_ << "\n";
    }

    void fight() {
        health_ -= 10;
        std::cout << name_ << " fought, health: " << health_ << "\n";
    }

    void levelUp() {
        level_++;
        std::cout << name_ << " leveled up to " << level_ << "\n";
    }

    void showState() const {
        std::cout << name_ << ": health=" << health_
                  << ", level=" << level_
                  << ", position=" << position_ << "\n";
    }

private:
    std::string name_;
    int health_;
    int level_;
    std::string position_;
};
```

### 3. Define the Caretaker

```cpp
class SaveManager {
public:
    void addMemento(std::unique_ptr<GameMemento> memento) {
        history_.push(std::move(memento));
    }

    GameMemento& getLatest() {
        return *history_.top();
    }

    GameMemento& undo() {
        if (history_.size() > 1) {
            history_.pop();
        }
        // history_.top() is a unique_ptr, so dereference it to return the memento.
        return *history_.top();
    }

    bool canUndo() const {
        return history_.size() > 1;
    }

    size_t historySize() const {
        return history_.size();
    }

private:
    std::stack<std::unique_ptr<GameMemento>> history_;
};
```

### 4. Client Usage

```cpp
int main() {
    Game player("Hero");
    SaveManager saveManager;

    // Initial state
    player.showState();

    // Save the initial state
    saveManager.addMemento(player.save());

    // Gameplay
    player.move("forest");
    player.fight();
    player.showState();

    // Save progress
    saveManager.addMemento(player.save());

    player.levelUp();
    player.move("castle");
    player.showState();

    // Save more progress
    saveManager.addMemento(player.save());

    std::cout << "\n=== Undo to previous state ===\n";
    player.restore(saveManager.undo());
    player.showState();

    std::cout << "\n=== History size: " << saveManager.historySize() << " ===\n";

    return 0;
}
```

## Modern C++ Implementations

### Using a Snapshot Manager

```cpp
class SnapshotManager {
public:
    template<typename T>
    void save(const T& state, const std::string& name) {
        snapshots_[name] = serialize(state);
    }

    template<typename T>
    T restore(const std::string& name) const {
        return deserialize<T>(snapshots_.at(name));
    }

private:
    std::map<std::string, std::string> snapshots_;

    template<typename T>
    std::string serialize(const T& state) {
        // Serialize with JSON or protobuf
        return "{}";
    }

    template<typename T>
    T deserialize(const std::string& data) {
        // Deserialize
        return T();
    }
};
```

### Using Version Control

```cpp
class VersionedMemento {
public:
    VersionedMemento(int version, const std::string& data)
        : version_(version), data_(data) {}

    int getVersion() const { return version_; }
    const std::string& getData() const { return data_; }

private:
    int version_;
    std::string data_;
};

class VersionManager {
public:
    void addVersion(std::unique_ptr<VersionedMemento> memento) {
        versions_.push_back(std::move(memento));
    }

    VersionedMemento& getLatest() {
        return *versions_.back();
    }

    VersionedMemento& getByVersion(int version) {
        for (const auto& v : versions_) {
            if (v->getVersion() == version) {
                return *v;
            }
        }
        throw std::out_of_range("Version not found");
    }

private:
    std::vector<std::unique_ptr<VersionedMemento>> versions_;
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|------|------|
| **Command Pattern** | The command pattern supports undo; the memento pattern saves state |
| **Prototype Pattern** | The prototype pattern clones an object; the memento pattern saves a state snapshot |
| **State Pattern** | The state pattern changes behavior; the memento pattern saves history states |

## Best Practices

### 1. Use a Deep Copy

```cpp
class Memento {
public:
    Memento(const State& state) {
        // Deep copy
        data_ = std::make_shared<StateData>(*state.getData());
    }

private:
    std::shared_ptr<StateData> data_;
};
```

### 2. Support Multiple Undo Steps

```cpp
class MultiUndoManager {
public:
    void addMemento(std::unique_ptr<Memento> memento) {
        future_.clear();  // drop the future states
        past_.push_back(std::move(memento));
    }

    void undo() {
        if (!past_.empty()) {
            future_.push_back(std::move(past_.back()));
            past_.pop_back();
        }
    }

    void redo() {
        if (!future_.empty()) {
            past_.push_back(std::move(future_.back()));
            future_.pop_back();
        }
    }

private:
    std::vector<std::unique_ptr<Memento>> past_;
    std::vector<std::unique_ptr<Memento>> future_;
};
```

### 3. Save Incrementally

```cpp
class IncrementalMemento {
public:
    void applyChange(const std::function<void(State&)>& change) {
        // Save the state before the change
        changes_.push_back(change);
    }

    void undo() {
        if (!changes_.empty()) {
            // Apply the last change in reverse
            auto change = std::move(changes_.back());
            changes_.pop_back();
            // Apply the inverse operation...
        }
    }

private:
    std::vector<std::function<void(State&)>> changes_;
};
```

## Common Traps

### Trap 1: Memory Leak

```cpp
// Not recommended: store a raw pointer
class BadManager {
public:
    void addMemento(Memento* memento) {
        memos_.push_back(memento);  // the delete is forgotten
    }

private:
    std::vector<Memento*> memos_;
};

// Recommended: use a smart pointer
class GoodManager {
public:
    void addMemento(std::unique_ptr<Memento> memento) {
        memos_.push_back(std::move(memento));
    }

private:
    std::vector<std::unique_ptr<Memento>> memos_;
};
```

### Trap 2: Inconsistent State

```cpp
// Not recommended: the memento and the original object drift apart
class InconsistentMemento {
public:
    InconsistentMemento(const Game& game)
        : health_(game.getHealth()) {}  // shallow copy

    int health_;  // does not follow the original object
};

// Recommended: snapshot the state on creation
class ConsistentMemento {
public:
    ConsistentMemento(const Game& game)
        : health_(game.getHealth()),
          level_(game.getLevel()),
          position_(game.getPosition()) {}
};
```

### Trap 3: Saving Too Often

```cpp
// Not recommended: save on every operation
void operation() {
    mementoManager.save(currentState);  // far too frequent
    doSomething();
}

// Recommended: save only at meaningful checkpoints
void operation() {
    doSomething();
    if (isCheckpoint()) {
        mementoManager.save(currentState);  // only at key points
    }
}
```

## Example

A runnable version of this topic:

- [memento-pattern-demo.cpp](../../examples/memento-pattern-demo.cpp)

## Summary

The key points of the memento pattern:

- **When to use it**: undo/redo, keeping history states, preserving encapsulation
- **Core benefits**: preserves encapsulation, supports undo, simplifies the originator
- **Implementation notes**: deep copy, smart pointer ownership, save at checkpoints
- **Common traps**: memory leaks, inconsistent state, saving too often

> **Remember**: the essence of the memento pattern is the "state snapshot" — saving
> and restoring an object's state without exposing its internal details.
