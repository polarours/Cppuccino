# Unit of Work Pattern

## What Is the Unit of Work Pattern?

Unit of Work is a data-access pattern that collects a group of business
operations into one atomic commit, coordinating every change to the database
through a single point.

> **Core idea**: keep a list of objects affected by a business transaction and
> coordinate writing the changes and resolving concurrency issues.

## How It Relates to the Repository Pattern

A repository manages access to a collection of a single entity; a unit of work
manages **one commit across several repositories**. The two are usually used
together:

```cpp
class UserRepository { /* ... */ };
class OrderRepository { /* ... */ };
class UnitOfWork { /* ... */ };  // full implementation below

class AppDbContext {
public:
    UserRepository users;   // repositories
    OrderRepository orders; // repositories
    UnitOfWork unitOfWork;  // the single commit point
};
```

## Core Concepts

- **Register**: record adds/changes/removes into a change set instead of
  writing to the database immediately
- **Commit**: write every change out once, in a defined order (the unit of work
  decides delete-before-insert and friends)
- **Rollback**: discard uncommitted changes

## Problem Scenario

```cpp
// Problem: every operation writes immediately — consistency not guaranteed
struct User { int id; };
struct Order { int id; };

struct Repo {
    template <typename T> bool save(const T&) { return true; }
};

Repo userRepo;
Repo orderRepo;

void transferOwnership(User& user, Order& order) {
    userRepo.save(user);    // succeeds
    orderRepo.save(order);  // fails → data now inconsistent
}
```

### Solution

```cpp
// Solution: register the changes, then commit once — all or nothing
struct User { int id; };
struct Order { int id; };

class UnitOfWork {  // full implementation in the section below
public:
    void registerModified(const User&) {}
    void registerModified(const Order&) {}
    bool commit() { return true; }  // all succeed or none take effect
};

void transferOwnership(UnitOfWork& uow, User& user, Order& order) {
    uow.registerModified(user);
    uow.registerModified(order);
    uow.commit();               // all succeed or none take effect
}
```

## Implementation

```cpp
#include <map>
#include <string>
#include <vector>

struct Entity {
    int id;
    std::string data;
};

enum class ChangeType { None, Add, Modify, Remove };

struct TrackedEntity {
    Entity entity;
    ChangeType changeType = ChangeType::None;
};

class UnitOfWork {
public:
    void registerNew(const Entity& entity) {
        tracked_[entity.id] = {entity, ChangeType::Add};
    }

    void registerModified(const Entity& entity) {
        auto it = tracked_.find(entity.id);
        if (it != tracked_.end() && it->second.changeType == ChangeType::Add) {
            it->second.entity = entity;  // added, then modified: still Add
        } else {
            tracked_[entity.id] = {entity, ChangeType::Modify};
        }
    }

    void registerRemoved(int id) {
        auto it = tracked_.find(id);
        if (it != tracked_.end() && it->second.changeType == ChangeType::Add) {
            tracked_.erase(it);          // added, then removed: cancel out
        } else {
            Entity dummy; dummy.id = id;
            tracked_[id] = {dummy, ChangeType::Remove};
        }
    }

    void commit() {
        for (auto& [id, tracked] : tracked_) {
            committed_.push_back(tracked);  // a real implementation writes the DB here
        }
        tracked_.clear();
    }

    void rollback() { tracked_.clear(); }
    bool hasChanges() const { return !tracked_.empty(); }

private:
    std::map<int, TrackedEntity> tracked_;
    std::vector<TrackedEntity> committed_;
};
```

For the full runnable example, see the
[unit-of-work project](../../cpp-mastery-roadmap/30-stage30/unit-of-work).

## Best Practices

1. Merge changes with correct state transitions (add then remove = no-op)
2. Commit order matters: usually Remove first, then Modify, finally Add (to
   avoid unique-key conflicts)
3. One business transaction = one UnitOfWork instance, discarded after use
4. Combine with an Identity Map to avoid loading the same row twice

## Example

A runnable version of this topic:

- [unit-of-work-demo.cpp](../../examples/unit-of-work-demo.cpp)

## Summary

By tracking changes centrally and deferring the write, Unit of Work turns
"many scattered writes" into "one atomic write" — the key structure for
business-transaction consistency.
