# Event Sourcing Pattern

## What Is the Event Sourcing Pattern?

Event sourcing is a persistence pattern that stores state as a sequence of
events that describe every change, instead of storing the current state.

> **Core idea**: never save the current state directly — save all the events
> that led to it. Current state becomes a projection you can rebuild at any
> time.

## How It Relates to CQRS / CRUD

| Aspect | CRUD | Event Sourcing | CQRS |
|--------|------|----------------|------|
| What is stored | Snapshot of current state | Sequence of state-changing events | Separate write model + read model |
| History / audit | Requires an extra audit table | Full timeline by construction | Depends on the underlying storage |
| Query pattern | Query tables directly | Replay events, then query the rebuilt state | Queries read purpose-built projections |
| Relationship | The opposite trade-off | Often paired with CQRS | Orthogonal: describes read/write split |

> **Division of labor with CQRS**: CQRS describes *separating the read model
> from the write model*; event sourcing describes *how the write side is
> persisted*. The event store becomes the write model and projections become
> the read model — the two compose naturally.

## Core Concepts

- **Event**: an immutable record of a state change
- **Aggregate**: the state rebuilt by folding the event sequence
- **Event Store**: the append-only storage that persists events

## Problem Scenario and Solution

**Bad**: only the latest balance is stored, so transfer history is lost — you
can never answer "when and by whom was this money changed?"

```cpp
// Anti-pattern: final state only — no audit, no timeline
struct AccountState { double balance; };
// balance = 120, but how did it get there? Not traceable.
```

**Good**: every transaction is an immutable event; account state is the
result of replaying the log.

```cpp
// Good: event log + fold replay
#include <variant>
#include <vector>

struct Deposit    { double amount; };
struct Withdrawal { double amount; };
using Txn = std::variant<Deposit, Withdrawal>;

double apply(double bal, const Txn& t) {
    return std::visit([&](const auto& e) {
        using T = std::decay_t<decltype(e)>;
        if constexpr (std::is_same_v<T, Deposit>) return bal + e.amount;
        else                                       return bal - e.amount;
    }, t);
}

double replay(const std::vector<Txn>& ledger) {
    double balance = 0;
    for (const auto& t : ledger) balance = apply(balance, t);
    return balance;  // state = fold(events)
}

// Usage: state is the result of replaying events
std::vector<Txn> ledger = {Deposit{100.0}, Withdrawal{30.0}, Deposit{50.0}};
// replay(ledger) == 120, and every step is auditable
```

## Implementation

```cpp
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <functional>

// Event base class
class Event {
public:
    virtual ~Event() = default;
    virtual std::string type() const = 0;
    virtual std::string serialize() const = 0;
};

// Concrete events
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

// Event store
class EventStore {
public:
    void append(const Event& event) {
        events_.push_back(&event);
    }

    const std::vector<const Event*>& getEvents() const { return events_; }

private:
    std::vector<const Event*> events_;
};

// Aggregate root
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

// Command handler
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

## Best Practices

1. Events are immutable
2. Version your events (schema evolution needs a migration story)
3. Project events into read-optimized models

## Example

A runnable version of this topic:

- [event-sourcing-demo.cpp](../../examples/event-sourcing-demo.cpp)

## Summary

By storing the full history of events, event sourcing provides a complete,
auditable timeline of every state change — at the cost of replay logic and
event versioning discipline.
