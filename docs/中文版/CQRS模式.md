# CQRS 模式

## 什么是 CQRS 模式？

CQRS（Command Query Responsibility Segregation，命令查询职责分离）把对系统的
修改（命令）和读取（查询）划分到不同的模型与通道上，而不是让同一份数据模型
同时承担两种职责。

> **核心思想**：写模型只服务于状态变更的正确性，读模型只服务于查询的效率——
> 两者各自演化，互不拖累。

## 与 Event Sourcing / CRUD 的关系

| 维度 | CRUD | Event Sourcing | CQRS |
|------|------|----------------|------|
| 模型数量 | 读写共用一个模型 | 一个事件流（读靠重建） | 读、写两个模型 |
| 写入方式 | 直接更新当前状态 | 追加事件 | 命令走写模型 |
| 查询方式 | 直接查表 | 重放事件后查询 | 查读模型投影 |
| 一致性 | 强一致 | 通常最终一致 | 写侧强一致，读侧可延迟 |
| 相互关系 | 基线方案 | 描述"写侧怎么存" | 描述"读写怎么分" |

三者是正交的：Event Sourcing 解决**持久化格式**，CQRS 解决**读写路径拆分**，
CRUD 是不作拆分的对照组。CQRS 常与事件溯源搭配（事件存储 = 写模型，投影 =
读模型），但 CQRS 不依赖事件溯源，普通表结构同样可以做。

## 核心概念

- **Command（命令）**：改变状态的操作，处理结果必须明确
- **Query（查询）**：只读，不改变状态，可以随时重复执行
- **Command Bus**：命令的分发通道，写侧入口
- **Query Bus**：查询的分发通道，读侧入口
- **读模型 / 写模型**：两者可以有完全不同的结构（范式化 vs 反范式化）

## 问题场景与解决方案

**问题**：读写共用一个模型。复杂报表查询被迫在写模型上现场聚合，既慢又会
和写操作互相拖累；想为查询做反范式化，又会污染写侧的范式结构。

```cpp
#include <string>
#include <vector>

// 反例：一个模型同时服务读和写
struct Order {
    int id;
    double amount;
};

struct OrderStore {
    std::vector<Order> orders;  // 写模型（范式化）

    // 复杂查询被迫在写模型上现场聚合，读写互相拖累
    std::string salesReport() {
        double total = 0;
        for (const auto& o : orders) total += o.amount;
        return "total:" + std::to_string(total);
    }
};
```

**方案**：写侧只维护规范化状态并同步更新读投影；读侧查询直接命中投影，
查询复杂度与写模型结构解耦。

```cpp
// 正例：写侧维护状态，读侧维护投影
#include <string>
#include <vector>

struct Order {
    int id;
    double amount;
};

struct WriteSide {
    std::vector<Order> orders;
    double total = 0;  // 读投影：写入时同步维护

    void place(int id, double amount) {
        orders.push_back({id, amount});
        total += amount;
    }
};

struct ReadSide {
    const double& total;  // 查询 O(1)，与写模型结构无关
};

int main() {
    WriteSide write;
    ReadSide read{write.total};
    write.place(1, 100.0);
    write.place(2, 50.0);
    return read.total == 150.0 ? 0 : 1;
}
```

## 代码实现

以下实现来自 [`cqrs-pattern` 项目](../../cpp-mastery-roadmap/28-stage28/cqrs-pattern/)，
命令与查询各走各的总线：写方法由命令处理器调用，读方法由查询处理器调用。

```cpp
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

namespace cqrs_pattern {

struct Account {
    int id;
    std::string name;
    double balance;
};

class CommandBus {
public:
    using Handler = std::function<void(const std::string&)>;

    void registerHandler(const std::string& commandType, Handler handler) {
        handlers_[commandType] = std::move(handler);
    }

    void dispatch(const std::string& commandType, const std::string& payload) {
        auto it = handlers_.find(commandType);
        if (it != handlers_.end()) {
            it->second(payload);
        }
    }

private:
    std::map<std::string, Handler> handlers_;
};

class QueryBus {
public:
    using Handler = std::function<std::string(const std::string&)>;

    void registerHandler(const std::string& queryType, Handler handler) {
        handlers_[queryType] = std::move(handler);
    }

    std::string query(const std::string& queryType, const std::string& params) {
        auto it = handlers_.find(queryType);
        if (it != handlers_.end()) {
            return it->second(params);
        }
        return "";
    }

private:
    std::map<std::string, Handler> handlers_;
};

class AccountService {
public:
    void createAccount(const std::string& name) {
        accounts_[nextId_] = {nextId_, name, 0.0};
        nextId_++;
    }

    void deposit(int id, double amount) {
        auto it = accounts_.find(id);
        if (it != accounts_.end()) {
            it->second.balance += amount;
        }
    }

    std::shared_ptr<Account> getAccount(int id) {
        auto it = accounts_.find(id);
        return (it != accounts_.end()) ? std::make_shared<Account>(it->second) : nullptr;
    }

    AccountService() {
        commands_.registerHandler("create", [this](const std::string& payload) {
            createAccount(payload);
        });
        commands_.registerHandler("deposit", [this](const std::string& payload) {
            auto pos = payload.find(':');
            int id = std::stoi(payload.substr(0, pos));
            double amount = std::stod(payload.substr(pos + 1));
            deposit(id, amount);
        });

        queries_.registerHandler("get", [this](const std::string& params) {
            int id = std::stoi(params);
            auto acc = getAccount(id);
            if (acc) return acc->name + ":" + std::to_string(acc->balance);
            return std::string();
        });
        queries_.registerHandler("list", [this](const std::string&) {
            std::string result;
            for (const auto& [id, acc] : accounts_) {
                if (!result.empty()) result += ",";
                result += acc.name;
            }
            return result;
        });
    }

    CommandBus& commands() { return commands_; }
    QueryBus& queries() { return queries_; }

private:
    std::map<int, Account> accounts_;
    int nextId_ = 1;
    CommandBus commands_;
    QueryBus queries_;
};

} // namespace cqrs_pattern

#include <cassert>
int main() {
    cqrs_pattern::AccountService svc;
    // 写侧：命令通道
    svc.commands().dispatch("create", "alice");
    svc.commands().dispatch("create", "bob");
    svc.commands().dispatch("deposit", "1:100");
    // 读侧：查询通道
    assert(svc.queries().query("get", "1") == "alice:100.000000");
    assert(svc.queries().query("get", "2") == "bob:0.000000");
    assert(svc.queries().query("list", "") == "alice,bob");
    // 只读查询可重复执行，不改变状态
    assert(svc.queries().query("get", "1") == "alice:100.000000");
    return 0;
}
```

## 最佳实践

1. **先判断是否真的需要 CQRS**：读写比例悬殊、两端扩展需求不同、需要独立
   演化时才值得；普通 CRUD 项目引入 CQRS 只会增加复杂度
2. **读模型允许延迟**：读侧投影可以异步更新，但要接受"写后立刻读不到最新值"
   的短暂窗口，或提供一致性兜底
3. **命令要表达意图**：用业务动词命名（`PlaceOrder` 而非 `Save`），一个命令
   只做一件事，失败要能明确返回
4. **查询无副作用**：查询处理器绝不修改状态，可以放心缓存、重试、并行执行
5. **与事件溯源搭配时**：事件存储作为唯一写模型，读模型由事件投影生成，
   投影可以随时丢弃重建

## 总结

CQRS 通过把命令和查询拆到不同模型与通道，让写侧专注一致性、读侧专注查询
效率；它与事件溯源是可组合的两件事，是否引入应由读写差异的真实程度决定。
