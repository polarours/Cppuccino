# 单元工作模式 (Unit of Work Pattern)

## 什么是单元工作模式？

单元工作模式（Unit of Work Pattern）是一种数据访问模式，它将一组业务操作收集为一次原子提交，统一协调对数据库的所有更改。

> **核心思想**：维护一个受业务事务影响的对象列表，协调变更的写入和并发问题的解决。

## 与仓储模式的关系

仓储模式管理单个实体的集合访问；单元工作模式管理**跨多个仓储的一次提交**。两者常配合使用：

```cpp
class AppDbContext {
public:
    UserRepository users;   // 仓储
    OrderRepository orders; // 仓储
    UnitOfWork unitOfWork;  // 统一提交点
};
```

## 核心概念

- **注册（Register）**：把新增/修改/删除记录到变更集，而不是立即写库
- **提交（Commit）**：一次性按顺序写出所有变更（先删后增等顺序由 UoW 决定）
- **回滚（Rollback）**：丢弃未提交的变更

## 问题场景

```cpp
// 问题：每个操作立即写库，无法保证一致性
void transferOwnership(User& user, Order& order) {
    userRepo.save(user);    // 成功
    orderRepo.save(order);  // 失败 → 数据不一致
}
```

### 解决方案

```cpp
void transferOwnership(UnitOfWork& uow, User& user, Order& order) {
    uow.registerModified(user);
    uow.registerModified(order);
    uow.commit();           // 全部成功或全部不生效
}
```

## 代码实现

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
            it->second.entity = entity;  // 新增后又改：仍是 Add
        } else {
            tracked_[entity.id] = {entity, ChangeType::Modify};
        }
    }

    void registerRemoved(int id) {
        auto it = tracked_.find(id);
        if (it != tracked_.end() && it->second.changeType == ChangeType::Add) {
            tracked_.erase(it);          // 新增后删除：直接抵消
        } else {
            Entity dummy; dummy.id = id;
            tracked_[id] = {dummy, ChangeType::Remove};
        }
    }

    void commit() {
        for (auto& [id, tracked] : tracked_) {
            committed_.push_back(tracked);  // 真实实现中此处写数据库
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

完整可运行示例见 [unit-of-work 项目](../../cpp-mastery-roadmap/30-stage30/unit-of-work)。

## 最佳实践

1. 变更合并要正确处理状态转移（新增后再删除 = 无操作）
2. 提交顺序很重要：通常先 Remove，再 Modify，最后 Add（避免唯一键冲突）
3. 一个业务事务对应一个 UnitOfWork 实例，用完即弃
4. 与身份映射（Identity Map）配合可避免重复加载同一行

## 总结

单元工作模式通过集中跟踪和延迟提交，把"多次零散写入"变成"一次原子写入"，是保证业务事务一致性的关键结构。
