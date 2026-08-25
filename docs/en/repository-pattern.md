# 仓储模式 (Repository Pattern)

## 什么是仓储模式？

仓储模式（Repository Pattern）是一种数据访问模式，它将数据访问逻辑封装在集合般的接口中，使业务逻辑与数据存储解耦。

> **核心思想**：用对象来模拟集合，将数据访问细节隐藏在仓储接口后面。

## 为什么需要仓储模式？

### 问题场景

```cpp
// 问题：业务逻辑直接操作数据库
class UserService {
public:
    User findById(int id) {
        // 直接 SQL 查询
        return db.query("SELECT * FROM users WHERE id = " + std::to_string(id));
    }
};
```

**问题**：业务逻辑耦合到数据访问细节

### 解决方案

```cpp
// 仓储接口
template<typename T>
class Repository {
public:
    virtual ~Repository() = default;
    virtual std::unique_ptr<T> findById(int id) = 0;
    virtual void save(const T& entity) = 0;
    virtual void deleteById(int id) = 0;
    virtual std::vector<T> findAll() = 0;
};

// 业务层只依赖接口
class UserService {
public:
    explicit UserService(std::unique_ptr<Repository<User>> repo)
        : repo_(std::move(repo)) {}
    
    User findUser(int id) {
        return *repo_->findById(id);
    }
};
```

## 代码实现

```cpp
#include <memory>
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

// 仓储接口
class UserRepository {
public:
    virtual ~UserRepository() = default;
    virtual std::unique_ptr<User> findById(int id) = 0;
    virtual void save(User user) = 0;
    virtual void deleteById(int id) = 0;
    virtual std::vector<User> findAll() = 0;
};

// 内存实现
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

// 业务服务
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

## 最佳实践

1. 一个实体对应一个仓储
2. 仓储接口专注于数据访问操作
3. 使用依赖注入便于测试

## 总结

仓储模式通过抽象数据访问层，使业务逻辑独立于存储实现。
