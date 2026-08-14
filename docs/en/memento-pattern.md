# 备忘录模式 (Memento Pattern)

## 什么是备忘录模式？

备忘录模式（Memento Pattern）是一种行为型设计模式，它允许在不暴露对象实现细节的前提下，保存和恢复对象的前一个状态。

> **核心思想**：保存对象的内部状态，以便在需要时恢复到该状态，同时保持封装性。

## 为什么需要备忘录模式？

### 问题场景

假设我们有一个文本编辑器，需要支持撤销操作：

```cpp
// 问题：如何保存和恢复编辑器状态？
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
    
    // 如何保存状态？
    // 如何恢复状态？
    
private:
    std::string typeface_;
    int size_ = 12;
    std::string content_;
};
```

**问题**：
1. 需要保存多个历史状态
2. 直接暴露内部状态破坏封装
3. 难以实现撤销/重做功能

### 解决方案

使用备忘录模式：

```cpp
// 备忘录：保存状态
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

// 发起人：创建和恢复备忘录
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

// 负责人：管理备忘录
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

**优点**：
1. 保持封装：内部状态不暴露
2. 支持撤销：可以保存多个状态
3. 简化发起人：不需要知道如何保存

## 模式结构

```
                    ┌─────────────┐
                    │  Caretaker  │
                    │  (负责人)    │
                    └──────┬──────┘
                           │ manages
                           │
              ┌────────────▼────────────┐
              │     Memento             │ ← 备忘录
              │   (EditorMemento)       │
              └────────────┬────────────┘
                           │
              ┌────────────▼────────────┐
              │      Originator         │ ← 发起人
              │     (TextEditor)        │
              └─────────────────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← 客户端
                    └─────────────────┘
```

## 代码实现

### 1. 定义备忘录

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

### 2. 定义发起人

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

### 3. 定义负责人

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
        return history_.top();
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

### 4. 客户端使用

```cpp
int main() {
    Game player("Hero");
    SaveManager saveManager;
    
    // 初始状态
    player.showState();
    
    // 保存初始状态
    saveManager.addMemento(player.save());
    
    // 游戏进行
    player.move("forest");
    player.fight();
    player.showState();
    
    // 保存进度
    saveManager.addMemento(player.save());
    
    player.levelUp();
    player.move("castle");
    player.showState();
    
    // 保存更多进度
    saveManager.addMemento(player.save());
    
    std::cout << "\n=== Undo to previous state ===\n";
    player.restore(saveManager.undo());
    player.showState();
    
    std::cout << "\n=== History size: " << saveManager.historySize() << " ===\n";
    
    return 0;
}
```

## 现代 C++ 实现

### 使用快照模式

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
        // 使用 JSON 或 protobuf 序列化
        return "{}";
    }
    
    template<typename T>
    T deserialize(const std::string& data) {
        // 反序列化
        return T();
    }
};
```

### 使用版本控制

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

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **命令模式** | 命令模式支持撤销；备忘录模式保存状态 |
| **原型模式** | 原型模式克隆对象；备忘录模式保存状态快照 |
| **状态模式** | 状态模式改变行为；备忘录模式保存历史状态 |

## 最佳实践

### 1. 使用深拷贝

```cpp
class Memento {
public:
    Memento(const State& state) {
        // 深拷贝
        data_ = std::make_shared<StateData>(*state.getData());
    }
    
private:
    std::shared_ptr<StateData> data_;
};
```

### 2. 支持多撤销

```cpp
class MultiUndoManager {
public:
    void addMemento(std::unique_ptr<Memento> memento) {
        future_.clear();  // 清除未来状态
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

### 3. 使用增量保存

```cpp
class IncrementalMemento {
public:
    void applyChange(const std::function<void(State&)>& change) {
        // 保存变化前的状态
        changes_.push_back(change);
    }
    
    void undo() {
        if (!changes_.empty()) {
            // 反向应用最后一个变化
            auto change = std::move(changes_.back());
            changes_.pop_back();
            // 应用反向操作...
        }
    }

private:
    std::vector<std::function<void(State&)>> changes_;
};
```

## 常见陷阱

### 陷阱 1：内存泄漏

```cpp
// 不推荐：存储原始指针
class BadManager {
public:
    void addMemento(Memento* memento) {
        memos_.push_back(memento);  // 忘记 delete
    }

private:
    std::vector<Memento*> memos_;
};

// 推荐：使用智能指针
class GoodManager {
public:
    void addMemento(std::unique_ptr<Memento> memento) {
        memos_.push_back(std::move(memento));
    }

private:
    std::vector<std::unique_ptr<Memento>> memos_;
};
```

### 陷阱 2：状态不一致

```cpp
// 不推荐：备忘录和原对象状态不同步
class InconsistentMemento {
public:
    InconsistentMemento(const Game& game) 
        : health_(game.getHealth()) {}  // 浅拷贝
    
    int health_;  // 不随原对象变化
};

// 推荐：创建时快照状态
class ConsistentMemento {
public:
    ConsistentMemento(const Game& game) 
        : health_(game.getHealth()),
          level_(game.getLevel()),
          position_(game.getPosition()) {}
};
```

### 陷阱 3：过度保存

```cpp
// 不推荐：每次操作都保存
void operation() {
    mementoManager.save(currentState);  // 太频繁
    doSomething();
}

// 推荐：只在关键状态保存
void operation() {
    doSomething();
    if (isCheckpoint()) {
        mementoManager.save(currentState);  // 关键节点
    }
}
```

## 总结

备忘录模式的核心要点：

- **适用场景**：需要撤销/重做、保存历史状态、保持封装
- **核心优点**：保持封装、支持撤销、简化发起人
- **实现要点**：深拷贝、智能指针管理、关键节点保存
- **常见陷阱**：内存泄漏、状态不一致、过度保存

> **记住**：备忘录模式的关键是"状态快照"——在不暴露内部细节的情况下保存和恢复对象状态。
