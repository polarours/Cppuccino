# 事件溯源模式 (Event Sourcing Pattern)

## 什么是事件溯源模式？

事件溯源（Event Sourcing）是一种持久化模式，它通过将状态改变记录为事件序列来存储状态。

> **核心思想**：不直接保存当前状态，而是保存导致状态变化的所有事件。

## 核心概念

- **事件（Event）**：状态改变的不可变记录
- **聚合（Aggregate）**：事件序列重组后的状态
- **事件存储（Event Store）**：持久化事件的存储

## 代码实现

```cpp
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <functional>

// 事件基类
class Event {
public:
    virtual ~Event() = default;
    virtual std::string type() const = 0;
    virtual std::string serialize() const = 0;
};

// 具体事件
class UserCreatedEvent : public Event {
public:
    std::string name;
    std::string email;
    
    std::string type() const override { return "UserCreated"; }
    std::string serialize() const override {
        return name + ":" + email;
    }
};

class UserUpdatedEvent : public Event {
public:
    std::string newName;
    
    std::string type() const override { return "UserUpdated"; }
    std::string serialize() const override { return newName; }
};

// 事件存储
class EventStore {
public:
    void append(const Event& event) {
        events_.push_back(&event);
    }
    
    const std::vector<const Event*>& getEvents() const { return events_; }

private:
    std::vector<const Event*> events_;
};

// 聚合根
class User {
public:
    int id;
    std::string name;
    std::string email;
    int version = 0;
    
    void apply(const Event& event) {
        if (auto* created = dynamic_cast<const UserCreatedEvent*>(&event)) {
            name = created->name;
            email = created->email;
        } else if (auto* updated = dynamic_cast<const UserUpdatedEvent*>(&event)) {
            name = updated->newName;
        }
        version++;
    }
};

// 命令处理器
class CommandHandler {
public:
    void handleCreate(const UserCreatedEvent& event, EventStore& store) {
        store.append(event);
        std::cout << "User created: " << event.name << "\n";
    }
    
    void handleUpdate(const UserUpdatedEvent& event, EventStore& store) {
        store.append(event);
        std::cout << "User updated: " << event.newName << "\n";
    }
};
```

## 最佳实践

1. 事件不可变
2. 使用事件版本控制
3. 投影到读取模型

## 总结

事件溯源通过保存事件历史，提供完整的状态变更审计能力。
