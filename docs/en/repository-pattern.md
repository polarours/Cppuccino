# Repository Pattern

## What Is the Repository Pattern?

The repository pattern is a data-access pattern that encapsulates data access
behind collection-like interfaces, decoupling business logic from the storage.

> **Core idea**: model a collection as an object and hide data-access details
> behind a repository interface.

## Why Do We Need It?

### Problem Scenario

```cpp
// Problem: business logic talks to the database directly
#include <string>

struct User { int id; std::string name; };

struct Db {
    User query(const std::string& sql) { return User{1, "alice"}; }
};

class UserService {
public:
    User findById(int id) {
        // raw SQL, written inside the business layer
        return db.query("SELECT * FROM users WHERE id = " + std::to_string(id));
    }
private:
    Db db;
};
```

**Problem**: business logic is welded to data-access details.

### Solution

```cpp
// Repository interface
#include <memory>
#include <string>
#include <vector>

struct User { int id; std::string name; };

template <typename T>
class Repository {
public:
    virtual ~Repository() = default;
    virtual std::unique_ptr<T> findById(int id) = 0;
    virtual void save(const T& entity) = 0;
    virtual void deleteById(int id) = 0;
    virtual std::vector<T> findAll() = 0;
};

// The business layer depends only on the interface
class UserService {
public:
    explicit UserService(std::unique_ptr<Repository<User>> repo)
        : repo_(std::move(repo)) {}

    User findUser(int id) {
        return *repo_->findById(id);
    }

private:
    std::unique_ptr<Repository<User>> repo_;
};
```

## Implementation

```cpp
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <iostream>

class User {
public:
    int id;
    std::string name;
    std::string email;
};

// Repository interface
class UserRepository {
public:
    virtual ~UserRepository() = default;
    virtual std::unique_ptr<User> findById(int id) = 0;
    virtual void save(User user) = 0;
    virtual void deleteById(int id) = 0;
    virtual std::vector<User> findAll() = 0;
};

// In-memory implementation
class InMemoryUserRepository : public UserRepository {
public:
    std::unique_ptr<User> findById(int id) override {
        auto it = store_.find(id);
        if (it != store_.end()) {
            return std::make_unique<User>(it->second);
        }
        return nullptr;
    }

    void save(User user) override {
        store_[user.id] = user;
    }

    void deleteById(int id) override {
        store_.erase(id);
    }

    std::vector<User> findAll() override {
        std::vector<User> result;
        for (const auto& [id, user] : store_) {
            result.push_back(user);
        }
        return result;
    }

private:
    std::map<int, User> store_;
};

// Business service
class UserService {
public:
    explicit UserService(std::unique_ptr<UserRepository> repo)
        : repo_(std::move(repo)) {}

    User findUser(int id) {
        auto user = repo_->findById(id);
        if (!user) throw std::runtime_error("User not found");
        return *user;
    }

    void createUser(const std::string& name, const std::string& email) {
        User user{next_id_++, name, email};
        repo_->save(user);
    }

    void deleteUser(int id) {
        repo_->deleteById(id);
    }

private:
    std::unique_ptr<UserRepository> repo_;
    int next_id_ = 1;
};
```

## Best Practices

1. One repository per entity
2. Keep the repository interface focused on data-access operations
3. Inject dependencies for testability

## Example

A runnable version of this topic:

- [repository-pattern-demo.cpp](../../examples/repository-pattern-demo.cpp)

## Summary

The repository pattern abstracts the data-access layer so business logic stays
independent of the storage implementation.
