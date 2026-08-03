# 命令模式 (Command Pattern)

## 什么是命令模式？

命令模式（Command Pattern）是一种行为型设计模式，它将请求封装成对象，从而允许使用不同的请求对客户端进行参数化，支持请求排队、记录日志、撤销操作等。

> **核心思想**：将动作的请求者与执行者解耦，通过命令对象传递动作。

## 为什么需要命令模式？

### 问题场景

假设我们有一个远程控制系统：

```cpp
// 问题：硬编码的遥控器逻辑
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
        // 每增加一个设备就要修改这个类
    }
};
```

**问题**：
1. 违反开闭原则：添加新设备需要修改遥控器
2. 耦合度高：遥控器知道所有设备
3. 无法撤销：没有状态保存
4. 无法记录历史：无法回放操作

### 解决方案

使用命令模式：

```cpp
// 抽象命令接口
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// 具体命令
class LightOnCommand : public Command {
public:
    explicit LightOnCommand(Light& light) : light_(light) {}
    
    void execute() override { light_.on(); }
    void undo() override { light_.off(); }

private:
    Light& light_;
};

// 遥控器
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

**优点**：
1. 解耦：遥控器不直接操作设备
2. 可扩展：添加新命令无需修改遥控器
3. 支持撤销：每个命令可以记录历史
4. 支持排队：可以批量执行命令
5. 支持日志：可以记录所有操作

## 模式结构

```
                    ┌─────────────┐
                    │  Invoker    │
                    │  (遥控器)    │
                    └──────┬──────┘
                           │
              ┌────────────▼────────────┐
              │       Command           │ ← 抽象命令接口
              │    (execute/undo)       │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  LightOn    │ │  LightOff   │ │   FanOn     │ ← 具体命令
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │       Receiver          │ ← 接收者（设备）
              │   (Light, Fan, TV...)   │
              └─────────────────────────┘
```

## 代码实现

### 1. 定义抽象命令

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

### 2. 实现具体命令

```cpp
// 设备类
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

// 具体命令：开灯
class LightOnCommand : public Command {
public:
    explicit LightOnCommand(Light& light) : light_(light) {}
    
    void execute() override { light_.on(); }
    void undo() override { light_.off(); }

private:
    Light& light_;
};

// 具体命令：关灯
class LightOffCommand : public Command {
public:
    explicit LightOffCommand(Light& light) : light_(light) {}
    
    void execute() override { light_.off(); }
    void undo() override { light_.on(); }

private:
    Light& light_;
};

// 具体命令：开风扇
class FanOnCommand : public Command {
public:
    explicit FanOnCommand(Fan& fan) : fan_(fan) {}
    
    void execute() override { fan_.on(); }
    void undo() override { fan_.off(); }

private:
    Fan& fan_;
};
```

### 3. 实现命令宏

```cpp
// 使用宏简化命令定义
#define DECLARE_COMMAND(class_name, receiver_type, action) \
class class_name : public Command { \
public: \
    explicit class_name(receiver_type& receiver) : receiver_(receiver) {} \
    void execute() override { receiver_.action(); } \
    void undo() override { receiver_.action##_off(); } \
private: \
    receiver_type& receiver_; \
};

// 使用宏
DECLARE_COMMAND(LightOnCmd, Light, on)
DECLARE_COMMAND(FanOnCmd, Fan, on)
```

### 4. 实现命令队列

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

### 5. 实现宏命令

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

### 6. 客户端使用

```cpp
int main() {
    Light light;
    Fan fan;
    
    LightOnCommand lightOn(light);
    LightOffCommand lightOff(light);
    FanOnCommand fanOn(fan);
    
    // 执行命令
    lightOn.execute();
    lightOn.execute();  // 多次执行
    lightOff.execute();
    
    // 撤销操作
    lightOn.execute();
    lightOn.undo();
    
    // 宏命令
    MacroCommand partyMode;
    partyMode.addCommand(std::make_unique<LightOnCommand>(light));
    partyMode.addCommand(std::make_unique<FanOnCommand>(fan));
    partyMode.execute();
    
    return 0;
}
```

## 现代 C++ 实现

### 使用 std::function

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

// 使用 lambda
ModernCommand lightOnCmd(
    [&light]() { light.on(); },
    [&light]() { light.off(); }
);
```

### 使用模板命令

```cpp
template<typename Receiver, typename Action>
class TemplateCommand : public Command {
public:
    TemplateCommand(Receiver& receiver, Action action)
        : receiver_(receiver), action_(std::move(action)) {}
    
    void execute() override { action_(receiver_); }
    void undo() override { /* 需要undo action */ }

private:
    Receiver& receiver_;
    Action action_;
};

// 使用
TemplateCommand<Light, void(Light&)> lightOnCmd(light, 
    [](Light& l) { l.on(); });
```

## 变体与扩展

### 变体 1：请求日志

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

### 变体 2：命令重放

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

### 变体 3：并发命令

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

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **策略模式** | 策略模式封装算法；命令模式封装请求 |
| **备忘录模式** | 备忘录保存状态；命令模式执行动作并支持撤销 |
| **迭代器模式** | 迭代器遍历集合；命令封装单个请求 |

## 最佳实践

### 1. 使用智能指针管理命令

```cpp
// 推荐：使用 unique_ptr
std::unique_ptr<Command> cmd = std::make_unique<LightOnCommand>(light);

// 不推荐：裸指针
Command* cmd = new LightOnCommand(light);
```

### 2. 确保撤销操作的一致性

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

### 3. 考虑命令的幂等性

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

## 常见陷阱

### 陷阱 1：撤销操作不可逆

```cpp
// 问题：某些操作无法撤销
class DeleteFileCommand : public Command {
public:
    void execute() override {
        std::remove(filename_.c_str());
    }
    
    void undo() override {
        // 无法恢复已删除的文件！
    }
};

// 推荐：确保可撤销
class SafeDeleteCommand : public Command {
public:
    void execute() override {
        moveToTrash(filename_);  // 移到回收站而非删除
    }
    
    void undo() override {
        restoreFromTrash(filename_);
    }
};
```

### 陷阱 2：命令对象生命周期

```cpp
// 问题：命令持有悬垂引用
class BadCommand : public Command {
public:
    explicit BadCommand(Light& light) : light_(light) {}  // 危险！

private:
    Light& light_;  // 引用可能悬垂
};

// 推荐：使用智能指针或值传递
class GoodCommand : public Command {
public:
    explicit GoodCommand(std::shared_ptr<Light> light) 
        : light_(std::move(light)) {}

private:
    std::shared_ptr<Light> light_;
};
```

### 陷阱 3：命令队列内存泄漏

```cpp
// 问题：命令执行后未释放
class Queue : public Command {
public:
    void execute() override {
        for (auto& cmd : queue_) {
            cmd->execute();
            // 未释放 cmd，导致内存泄漏
        }
    }

private:
    std::vector<Command*> queue_;
};

// 推荐：使用 unique_ptr 自动管理
class SafeQueue : public Command {
public:
    void execute() override {
        for (auto& cmd : queue_) {
            cmd->execute();
        }
        queue_.clear();  // 自动释放
    }

private:
    std::vector<std::unique_ptr<Command>> queue_;
};
```

## 总结

命令模式的核心要点：

- **适用场景**：需要撤销/重做、请求排队、日志记录、宏命令
- **核心优点**：解耦、可扩展、支持撤销、支持日志
- **实现要点**：使用智能指针、确保可撤销、考虑生命周期
- **常见陷阱**：不可撤销操作、悬垂引用、内存泄漏

> **记住**：命令模式的关键是"封装请求"——将请求封装成对象，使请求可以像其他对象一样被传递、存储和操作。
