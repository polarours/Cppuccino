# 活动记录模式 (Active Record Pattern)

## 什么是活动记录模式？

活动记录模式（Active Record Pattern）是一种数据访问模式：一个对象既封装业务数据，又封装持久化操作。表的一行对应对象的一个实例。

> **核心思想**：让实体自己知道如何 `save()`、`remove()` 和静态地 `find()` 自己。

## 与仓储模式的对比

| | Active Record | Repository |
|---|---|---|
| 数据与访问逻辑 | 合一（实体自带 CRUD） | 分离（仓储负责持久化） |
| 业务层依赖 | 直接依赖具体实体类 | 只依赖仓储接口 |
| 可测试性 | 较弱（实体耦合存储） | 较强（可注入内存实现） |
| 适用场景 | 简单 CRUD、脚本、小型服务 | 复杂领域逻辑、需要换存储 |

## 核心概念

- **脏标记（Dirty Flag）**：字段被修改后标记为 dirty，保存时才知道要写库
- **新记录判定**：`id == 0` 表示未持久化，`save()` 时走 INSERT 而非 UPDATE
- **静态注册表**：`find/findAll` 从注册表（模拟数据库）取回实例

## 代码实现

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

完整可运行示例见 [active-record 项目](../../cpp-mastery-roadmap/30-stage30/active-record)。

## 使用示例

```cpp
UserRecord::clearRegistry();

UserRecord user("Alice", "alice@example.com");
user.save();                                  // INSERT
user.setName("Alice Smith");
assert(user.isDirty());
user.save();                                  // UPDATE

auto found = UserRecord::find(1);
assert(found && found->name() == "Alice Smith");
```

## 最佳实践

1. setter 中自动 `markDirty()`，避免调用方忘记标记
2. 静态成员用 `static inline`（C++17 起）避免单独定义
3. 实体逻辑复杂后应及时迁移到 Repository + 领域对象，避免"胖模型"
4. 多线程环境下注意静态注册表的并发访问

## 总结

活动记录模式以最少的层换取最直接的持久化体验；简单场景下非常高效，但数据与存储的耦合限制了它在复杂系统中的扩展性。
