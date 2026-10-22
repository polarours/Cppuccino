# Mediator Pattern

## What Is the Mediator Pattern?

The Mediator Pattern is a behavioral design pattern that encapsulates the
interactions between a set of objects by introducing a mediator object. The
objects no longer refer to each other explicitly, which lowers the coupling.

> **Core idea**: turn the many-to-many relationships between objects into
> one-to-many relationships with a mediator, decoupling the objects.

## Why Do We Need the Mediator Pattern?

### Problem Scenario

Suppose we have a chat room system where multiple users talk to each other:

```cpp
// Problem: direct communication between objects creates heavy coupling
class ChatRoom {
public:
    void sendMessage(const std::string& sender,
                     const std::string& receiver,
                     const std::string& message) {
        // It has to know every user
        if (receiver == "Alice") {
            alice_.receive(sender, message);
        } else if (receiver == "Bob") {
            bob_.receive(sender, message);
        }
        // ... every new user requires another edit here
    }

private:
    User alice_;
    User bob_;
    User charlie_;
    // The more users there are, the more complicated this gets
};
```

**Problems**:
1. The objects are tightly coupled
2. Adding a new user requires touching all the related code
3. Hard to maintain and to test

### Solution

Use the mediator pattern:

```cpp
// Mediator interface
class ChatMediator {
public:
    virtual ~ChatMediator() = default;
    virtual void registerUser(User* user) = 0;
    virtual void sendMessage(const std::string& sender,
                             const std::string& receiver,
                             const std::string& message) = 0;
};

// Concrete mediator
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

// A user only talks to the mediator
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

**Benefits**:
1. Decoupling: the objects do not need to know each other
2. Extensibility: new users are easy to add
3. Centralized control: the mediator manages every interaction

## Pattern Structure

```
                    ┌─────────────┐
                    │   Mediator  │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │ Concrete    │ │Concrete    │ │ Concrete   │ ← concrete mediator
    │ Mediator    │ │ Mediator   │ │ Mediator   │
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │       Colleague         │ ← colleague interface
              │      (User)             │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │   User A    │ │   User B    │ │   User C    │ ← concrete colleagues
    └─────────────┘ └─────────────┘ └─────────────┘
```

## Implementation

### 1. Define the Mediator Interface

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <functional>

class Colleague;  // forward declaration

class Mediator {
public:
    virtual ~Mediator() = default;
    virtual void registerColleague(Colleague* colleague) = 0;
    virtual void notify(const std::string& sender,
                        const std::string& message) = 0;
    virtual void sendMessage(Colleague* sender,
                             Colleague* receiver,
                             const std::string& message) = 0;
    // The mediator owns the directory, so it resolves a name to a colleague.
    virtual Colleague* findColleague(const std::string& name) = 0;
};
```

### 2. Define the Colleague Interface

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

### 3. Implement the Concrete Mediator

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

    Colleague* findColleague(const std::string& name) override {
        auto it = colleagues_.find(name);
        return it != colleagues_.end() ? it->second : nullptr;
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

### 4. Implement the Concrete Colleague

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
            // The mediator owns the directory, so it resolves the receiver.
            mediator_->sendMessage(this, mediator_->findColleague(receiver), message);
        }
    }

    std::string getName() const override { return name_; }

private:
    std::string name_;
    Mediator* mediator_ = nullptr;
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
        // A bot can answer automatically
        if (message.find("help") != std::string::npos) {
            std::cout << name_ << ": I can help you!\n";
        }
    }

    void send(const std::string& receiver,
              const std::string& message) override {
        // A bot sends messages
        (void)receiver;
        (void)message;
    }

    std::string getName() const override { return name_; }

private:
    std::string name_;
    Mediator* mediator_ = nullptr;
};
```

### 5. Client Usage

```cpp
int main() {
    ChatMediator mediator;

    // Create the users
    User alice("Alice");
    User bob("Bob");
    User charlie("Charlie");
    Bot bot("Bot");

    // Register them with the mediator
    alice.setMediator(&mediator);
    bob.setMediator(&mediator);
    charlie.setMediator(&mediator);
    bot.setMediator(&mediator);

    mediator.registerColleague(&alice);
    mediator.registerColleague(&bob);
    mediator.registerColleague(&charlie);
    mediator.registerColleague(&bot);

    // List every user
    mediator.listUsers();

    // Send messages
    std::cout << "\n=== Chat Demo ===\n";
    alice.send("Bob", "Hello Bob!");
    bob.send("Alice", "Hi Alice!");
    charlie.send("Alice", "Hello everyone!");

    // Broadcast a message
    std::cout << "\n=== Broadcast Demo ===\n";
    mediator.notify("Alice", "Meeting at 3 PM");

    return 0;
}
```

## Modern C++ Implementations

### Using Smart Pointers

```cpp
class SmartMediator : public Mediator {
public:
    void registerColleague(Colleague* colleague) override {
        colleagues_[colleague->getName()] = colleague;
    }

    void notify(const std::string& sender,
                const std::string& message) override {
        for (auto& [name, col] : colleagues_) {
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

    Colleague* findColleague(const std::string& name) override {
        auto it = colleagues_.find(name);
        return it != colleagues_.end() ? it->second : nullptr;
    }

private:
    std::map<std::string, Colleague*> colleagues_;
};
```

### Using an Event System

```cpp
class EventMediator {
public:
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> handler) {
        handlers_[typeid(EventType).name()].push_back(
            [handler](const void* payload) {
                handler(*static_cast<const EventType*>(payload));
            });
    }

    template<typename EventType>
    void publish(const EventType& event) {
        auto it = handlers_.find(typeid(EventType).name());
        if (it != handlers_.end()) {
            for (auto& handler : it->second) {
                handler(&event);
            }
        }
    }

private:
    std::map<std::string,
             std::vector<std::function<void(const void*)>>> handlers_;
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|------|------|
| **Observer Pattern** | The observer is a one-to-many notification; the mediator coordinates many-to-many interactions |
| **Singleton Pattern** | A singleton has one instance; a mediator manages several objects |
| **Facade Pattern** | A facade simplifies an interface; a mediator coordinates interactions |

## Best Practices

### 1. Keep the Mediator Simple

```cpp
// Not recommended: the mediator accumulates too much business logic
class BadMediator {
public:
    void process() {
        // far too much logic...
    }
};

// Recommended: the mediator only coordinates
class GoodMediator {
public:
    void forwardMessage(Colleague* sender,
                        Colleague* receiver,
                        const std::string& msg) {
        receiver->receive(sender->getName(), msg);
    }
};
```

### 2. Use Dependency Injection

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

### 3. Support Dynamic Registration

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

    void notify(const std::string& sender, const std::string& message) override {
        for (const auto& [name, col] : colleagues_) {
            if (name != sender) {
                col->receive(sender, message);
            }
        }
    }

    void sendMessage(Colleague* sender, Colleague* receiver,
                     const std::string& message) override {
        if (receiver) {
            receiver->receive(sender->getName(), message);
        }
    }

    Colleague* findColleague(const std::string& name) override {
        auto it = colleagues_.find(name);
        return it != colleagues_.end() ? it->second : nullptr;
    }

private:
    std::map<std::string, Colleague*> colleagues_;
};
```

## Common Traps

### Trap 1: God Mediator

```cpp
// Not recommended: the mediator holds all of the logic
class GodMediator {
public:
    void processAll() {
        // too many methods, hard to maintain
        handleUserA();
        handleUserB();
        handleUserC();
        // ...
    }
};

// Recommended: split it into several small mediators
class UserMediator { ... };
class MessageMediator { ... };
```

### Trap 2: Circular References

```cpp
// Not recommended: the mediator and the colleagues refer to each other
class BadMediator {
    Colleague* colleague_;  // risk of a circular reference
};

class BadColleague {
    Mediator* mediator_;    // risk of a circular reference
};

// Recommended: use weak references
class GoodMediator {
    std::weak_ptr<Colleague> colleague_;
};
```

### Trap 3: Performance

```cpp
// Not recommended: walk every colleague for each message
void notifyAll(const std::string& message) {
    for (auto& col : colleagues_) {
        col->receive(message);  // O(n)
    }
}

// Recommended: use a publish/subscribe model
void publish(const std::string& event) {
    auto it = handlers_.find(event);
    if (it != handlers_.end()) {
        for (auto& handler : it->second) {
            handler();
        }
    }
}
```

## Example

A runnable version of this topic:

- [mediator-pattern-demo.cpp](../../examples/mediator-pattern-demo.cpp)

## Summary

The key points of the mediator pattern:

- **When to use it**: heavy coupling between objects, coordinating multi-object
  interactions, lowering coupling
- **Core benefits**: decoupling, centralized control, easy to maintain
- **Implementation notes**: keep the mediator simple, use smart pointers, support
  dynamic registration
- **Common traps**: God Mediator, circular references, performance

> **Remember**: the essence of the mediator pattern is "coordinating interactions" —
> the mediator object manages the interactions between objects so they never have to
> talk to each other directly.
