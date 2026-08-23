# 中介者模式 (Mediator Pattern)

## 什么是中介者模式？

中介者模式（Mediator Pattern）是一种行为型设计模式，它通过引入一个中介对象来封装一系列对象之间的交互，使各对象不需要显式相互引用，从而降低耦合度。

> **核心思想**：将对象间的多对多关系转变为中介者的一对多关系，实现对象间的解耦。

## 为什么需要中介者模式？

### 问题场景

假设我们有一个聊天室系统，多个用户可以互相通信：

```cpp
// 问题：对象间直接通信导致耦合严重
class ChatRoom {
public:
    void sendMessage(const std::string& sender, 
                     const std::string& receiver, 
                     const std::string& message) {
        // 需要知道所有用户
        if (receiver == "Alice") {
            alice_.receive(sender, message);
        } else if (receiver == "Bob") {
            bob_.receive(sender, message);
        }
        // ... 每添加一个用户都要修改这里
    }

private:
    User alice_;
    User bob_;
    User charlie_;
    // 用户越多，代码越复杂
};
```

**问题**：
1. 对象间紧密耦合
2. 添加新用户需要修改所有相关代码
3. 难以维护和测试

### 解决方案

使用中介者模式：

```cpp
// 中介者接口
class ChatMediator {
public:
    virtual ~ChatMediator() = default;
    virtual void registerUser(User* user) = 0;
    virtual void sendMessage(const std::string& sender, 
                             const std::string& receiver,
                             const std::string& message) = 0;
};

// 具体中介者
class ChatRoomMediator : public ChatMediator {
public:
    void registerUser(User* user) override {
        users_[user->getName()] = user;
    }
    
    void sendMessage(const std::string& sender, 
                     const std::string& receiver,
                     const std::string& message) override {
        auto it = users_.find(receiver);
        if (it != users_.end()) {
            it->second->receive(sender, message);
        }
    }

private:
    std::map<std::string, User*> users_;
};

// 用户只与中介者通信
class User {
public:
    User(ChatMediator& mediator, const std::string& name)
        : mediator_(mediator), name_(name) {}
    
    void send(const std::string& receiver, const std::string& message) {
        mediator_.sendMessage(name_, receiver, message);
    }
    
    void receive(const std::string& sender, const std::string& message) {
        std::cout << name_ << " received from " << sender 
                  << ": " << message << "\n";
    }

private:
    ChatMediator& mediator_;
    std::string name_;
};
```

**优点**：
1. 解耦：对象不需要知道其他对象
2. 可扩展：轻松添加新用户
3. 集中控制：中介者管理所有交互

## 模式结构

```
                    ┌─────────────┐
                    │   Mediator  │
                    │  (中介者)    │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │ Concrete    │ │Concrete    │ │ Concrete   │ ← 具体中介者
    │ Mediator    │ │ Mediator   │ │ Mediator   │
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │       Colleague         │ ← 同事接口
              │      (User)             │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │   User A    │ │   User B    │ │   User C    │ ← 具体同事
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 代码实现

### 1. 定义中介者接口

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <functional>

class Colleague;  // 前向声明

class Mediator {
public:
    virtual ~Mediator() = default;
    virtual void registerColleague(Colleague* colleague) = 0;
    virtual void notify(const std::string& sender, 
                        const std::string& message) = 0;
    virtual void sendMessage(Colleague* sender, 
                             Colleague* receiver,
                             const std::string& message) = 0;
};
```

### 2. 定义同事接口

```cpp
class Colleague {
public:
    virtual ~Colleague() = default;
    virtual std::string getName() const = 0;
    virtual void setMediator(Mediator* mediator) = 0;
    virtual void receive(const std::string& sender, 
                         const std::string& message) = 0;
    virtual void send(const std::string& receiver, 
                      const std::string& message) = 0;
};
```

### 3. 实现具体中介者

```cpp
class ChatMediator : public Mediator {
public:
    void registerColleague(Colleague* colleague) override {
        colleagues_[colleague->getName()] = colleague;
    }
    
    void notify(const std::string& sender, 
                const std::string& message) override {
        std::cout << "[Broadcast] " << sender << ": " << message << "\n";
        for (const auto& [name, col] : colleagues_) {
            if (name != sender) {
                col->receive(sender, message);
            }
        }
    }
    
    void sendMessage(Colleague* sender, 
                     Colleague* receiver,
                     const std::string& message) override {
        if (receiver) {
            receiver->receive(sender->getName(), message);
        }
    }
    
    void listUsers() const {
        std::cout << "Users in chat: ";
        for (const auto& [name, _] : colleagues_) {
            std::cout << name << " ";
        }
        std::cout << "\n";
    }

private:
    std::map<std::string, Colleague*> colleagues_;
};
```

### 4. 实现具体同事

```cpp
class User : public Colleague {
public:
    explicit User(const std::string& name) : name_(name) {}
    
    void setMediator(Mediator* mediator) override {
        mediator_ = mediator;
    }
    
    void receive(const std::string& sender, 
                 const std::string& message) override {
        std::cout << name_ << " received from " << sender 
                  << ": " << message << "\n";
    }
    
    void send(const std::string& receiver, 
              const std::string& message) override {
        if (mediator_) {
            auto it = colleagues_.find(receiver);
            if (it != colleagues_.end()) {
                Colleague* target = nullptr;
                // 在真实实现中通过中介者查找
                mediator_->sendMessage(this, target, message);
            }
        }
    }
    
    std::string getName() const override { return name_; }
    void addColleague(Colleague* colleague) {
        colleagues_[colleague->getName()] = colleague;
    }

private:
    std::string name_;
    Mediator* mediator_ = nullptr;
    std::map<std::string, Colleague*> colleagues_;
};

class Bot : public Colleague {
public:
    explicit Bot(const std::string& name) : name_(name) {}
    
    void setMediator(Mediator* mediator) override {
        mediator_ = mediator;
    }
    
    void receive(const std::string& sender, 
                 const std::string& message) override {
        std::cout << name_ << " received: " << message << "\n";
        // 机器人可以自动回复
        if (message.find("help") != std::string::npos) {
            std::cout << name_ << ": I can help you!\n";
        }
    }
    
    void send(const std::string& receiver, 
              const std::string& message) override {
        // Bot 发送消息
        (void)receiver;
        (void)message;
    }
    
    std::string getName() const override { return name_; }

private:
    std::string name_;
    Mediator* mediator_ = nullptr;
};
```

### 5. 客户端使用

```cpp
int main() {
    ChatMediator mediator;
    
    // 创建用户
    User alice("Alice");
    User bob("Bob");
    User charlie("Charlie");
    Bot bot("Bot");
    
    // 注册到中介者
    alice.setMediator(&mediator);
    bob.setMediator(&mediator);
    charlie.setMediator(&mediator);
    bot.setMediator(&mediator);
    
    mediator.registerColleague(&alice);
    mediator.registerColleague(&bob);
    mediator.registerColleague(&charlie);
    mediator.registerColleague(&bot);
    
    // 列出所有用户
    mediator.listUsers();
    
    // 发送消息
    std::cout << "\n=== Chat Demo ===\n";
    alice.send("Bob", "Hello Bob!");
    bob.send("Alice", "Hi Alice!");
    charlie.send("Alice", "Hello everyone!");
    
    // 广播消息
    std::cout << "\n=== Broadcast Demo ===\n";
    mediator.notify("Alice", "Meeting at 3 PM");
    
    return 0;
}
```

## 现代 C++ 实现

### 使用智能指针

```cpp
class SmartMediator : public Mediator {
public:
    void registerColleague(std::unique_ptr<Colleague> colleague) {
        colleagues_[colleague->getName()] = std::move(colleague);
    }
    
    void notify(const std::string& sender, 
                const std::string& message) override {
        for (auto& [name, col] : colleagues_) {
            if (name != sender) {
                col->receive(sender, message);
            }
        }
    }

private:
    std::map<std::string, std::unique_ptr<Colleague>> colleagues_;
};
```

### 使用事件系统

```cpp
class EventMediator {
public:
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> handler) {
        handlers_[typeid(EventType).name()].push_back(handler);
    }
    
    template<typename EventType>
    void publish(const EventType& event) {
        auto it = handlers_.find(typeid(EventType).name());
        if (it != handlers_.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

private:
    std::map<std::string, 
             std::vector<std::function<void(const void*)>>> handlers_;
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **观察者模式** | 观察者是一对多通知；中介者是多对多协调 |
| **单例模式** | 单例只有一个实例；中介者管理多个对象 |
| **门面模式** | 门面简化接口；中介者协调交互 |

## 最佳实践

### 1. 保持中介者简单

```cpp
// 不推荐：中介者包含过多业务逻辑
class BadMediator {
public:
    void process() {
        // 太多逻辑...
    }
};

// 推荐：中介者只负责协调
class GoodMediator {
public:
    void forwardMessage(Colleague* sender, 
                        Colleague* receiver, 
                        const std::string& msg) {
        receiver->receive(sender->getName(), msg);
    }
};
```

### 2. 使用依赖注入

```cpp
class MediatorSystem {
public:
    void setMediator(std::unique_ptr<Mediator> mediator) {
        mediator_ = std::move(mediator);
    }
    
    void broadcast(const std::string& message) {
        mediator_->notify("system", message);
    }

private:
    std::unique_ptr<Mediator> mediator_;
};
```

### 3. 支持动态注册

```cpp
class DynamicMediator : public Mediator {
public:
    bool registerColleague(Colleague* colleague) {
        if (colleague && colleagues_.find(colleague->getName()) == colleagues_.end()) {
            colleagues_[colleague->getName()] = colleague;
            return true;
        }
        return false;
    }
    
    bool unregisterColleague(const std::string& name) {
        return colleagues_.erase(name) > 0;
    }

private:
    std::map<std::string, Colleague*> colleagues_;
};
```

## 常见陷阱

### 陷阱 1：God Mediator

```cpp
// 不推荐：中介者包含所有逻辑
class GodMediator {
public:
    void processAll() {
        // 太多方法，难以维护
        handleUserA();
        handleUserB();
        handleUserC();
        // ...
    }
};

// 推荐：拆分成多个小中介者
class UserMediator { ... };
class MessageMediator { ... };
```

### 陷阱 2：循环引用

```cpp
// 不推荐：中介者和同事互相引用
class BadMediator {
    Colleague* colleague_;  // 循环引用风险
};

class BadColleague {
    Mediator* mediator_;    // 循环引用风险
};

// 推荐：使用弱引用
class GoodMediator {
    std::weak_ptr<Colleague> colleague_;
};
```

### 陷阱 3：性能问题

```cpp
// 不推荐：每次消息都遍历所有同事
void notifyAll(const std::string& message) {
    for (auto& col : colleagues_) {
        col->receive(message);  // O(n)
    }
}

// 推荐：使用发布订阅模式
void publish(const std::string& event) {
    auto it = handlers_.find(event);
    if (it != handlers_.end()) {
        for (auto& handler : it->second) {
            handler();
        }
    }
}
```

## 总结

中介者模式的核心要点：

- **适用场景**：对象间耦合严重、需要协调多对象交互、降低耦合度
- **核心优点**：解耦、集中控制、易于维护
- **实现要点**：保持中介者简单、使用智能指针、支持动态注册
- **常见陷阱**：God Mediator、循环引用、性能问题

> **记住**：中介者模式的关键是"协调交互"——通过中介者对象来管理对象间的交互，使它们不需要直接通信。
