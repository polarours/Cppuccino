# Active Record Pattern

## What Is the Active Record Pattern?

Active Record is a data-access pattern: one object encapsulates both the
business data and the persistence operations. A table row corresponds to one
object instance.

> **Core idea**: the entity itself knows how to `save()`, `remove()`, and
> statically `find()` itself.

## Active Record vs Repository

| | Active Record | Repository |
|---|---|---|
| Data and access logic | Combined (entity carries CRUD) | Separated (repository owns persistence) |
| Business-layer dependency | Depends directly on concrete entity classes | Depends only on the repository interface |
| Testability | Weaker (entity coupled to storage) | Stronger (an in-memory implementation can be injected) |
| Fit | Simple CRUD, scripts, small services | Complex domain logic, storage must be swappable |

## Core Concepts

- **Dirty flag**: fields are marked dirty when modified, so `save()` knows
  what to write
- **New-record detection**: `id == 0` means not yet persisted; `save()` takes
  the INSERT path instead of UPDATE
- **Static registry**: `find/findAll` retrieve instances from the registry
  (a stand-in database)

## Implementation

```cpp
#include <map>
#include <memory>
#include <string>
#include <vector>

class ActiveRecord {
public:
    virtual ~ActiveRecord() = default;

    bool isNew() const { return id_ == 0; }
    int id() const { return id_; }
    void setId(int id) { id_ = id; }
    bool isDirty() const { return dirty_; }
    void markClean() { dirty_ = false; }

protected:
    void markDirty() { dirty_ = true; }

private:
    int id_ = 0;
    bool dirty_ = false;
};

class UserRecord : public ActiveRecord {
public:
    UserRecord() = default;
    UserRecord(std::string name, std::string email)
        : name_(std::move(name)), email_(std::move(email)) {}

    const std::string& name() const { return name_; }

    void setName(const std::string& name) { name_ = name; markDirty(); }

    bool save() {
        if (isNew()) {
            setId(nextId_++);
        }
        registry_[id()] = std::make_shared<UserRecord>(*this);
        markClean();
        return true;
    }

    static std::shared_ptr<UserRecord> find(int id) {
        auto it = registry_.find(id);
        return (it != registry_.end()) ? it->second : nullptr;
    }

    static std::size_t count() { return registry_.size(); }

    static void clearRegistry() { registry_.clear(); nextId_ = 1; }

private:
    std::string name_;
    std::string email_;
    static inline int nextId_ = 1;
    static inline std::map<int, std::shared_ptr<UserRecord>> registry_;
};
```

For the full runnable example, see the
[active-record project](../../cpp-mastery-roadmap/30-stage30/active-record).

## Usage Example

```cpp
#include <cassert>

UserRecord::clearRegistry();

UserRecord user("Alice", "alice@example.com");
user.save();                                  // INSERT
user.setName("Alice Smith");
assert(user.isDirty());
user.save();                                  // UPDATE

auto found = UserRecord::find(1);
assert(found && found->name() == "Alice Smith");
```

## Best Practices

1. Call `markDirty()` inside setters so callers cannot forget to mark
2. Use `static inline` members (C++17 and later) to avoid separate definitions
3. Once entity logic grows complex, migrate to Repository + domain objects
   before the model becomes "fat"
4. Under concurrency, watch out for concurrent access to the static registry

## Example

A runnable version of this topic:

- [active-record-demo.cpp](../../examples/active-record-demo.cpp)

## Summary

Active Record trades layers for the most direct persistence experience: very
efficient in simple scenarios, but the coupling of data to storage limits how
well it scales into complex systems.
