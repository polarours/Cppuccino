# C++ 测试模式 (Testing Patterns in C++)

## 为什么需要测试模式？

单元测试的目标是**隔离**、**可重复**、**快速**地验证代码逻辑。C++ 没有内置测试框架，社区惯用 Google Test、Boost.Test、Catch2 等，但无论选哪个框架，三种核心测试模式——**Mock Objects**、**Test Fixture**、**Parameterized Tests**——都通用。

> **核心目标**：把"被测代码"和"其依赖"分开，让测试只验证一件事。

## 1. Mock Objects（模拟对象）

### 什么是 Mock？

Mock 是测试替身（Test Double）的一种，它**记录调用**但不真正执行副作用。常用于验证两个对象间的交互（调用次数、参数、顺序），而不需要真实的数据库/网络/文件。

> Mock 记录调用 → Stub 返回预设值 → Fake 是轻量仿实现

### 代码实现

```cpp
#include <functional>
#include <string>
#include <vector>

class Database {
public:
    virtual ~Database() = default;
    virtual bool connect(const std::string& url) = 0;
    virtual bool query(const std::string& sql, std::string& result) = 0;
    virtual void disconnect() = 0;
};

class MockDatabase : public Database {
public:
    bool connect(const std::string& url) override {
        connectCalls_.push_back(url);
        return shouldSucceed_;
    }

    bool query(const std::string& sql, std::string& result) override {
        queryCalls_.push_back(sql);
        if (queryHandler_) return queryHandler_(sql, result);
        result = lastQueryResult_;
        return shouldSucceed_;
    }

    void disconnect() override { disconnectCalled_ = true; }

    // --- Mock 控制点 ---
    void setShouldSucceed(bool v) { shouldSucceed_ = v; }
    void setLastQueryResult(const std::string& r) { lastQueryResult_ = r; }
    void setQueryHandler(std::function<bool(const std::string&, std::string&)> h) {
        queryHandler_ = std::move(h);
    }

    // --- 断言点 ---
    std::size_t connectCallCount() const { return connectCalls_.size(); }
    std::size_t queryCallCount() const { return queryCalls_.size(); }
    bool disconnectCalled() const { return disconnectCalled_; }

private:
    bool shouldSucceed_ = true;
    bool disconnectCalled_ = false;
    std::string lastQueryResult_;
    std::vector<std::string> connectCalls_;
    std::vector<std::string> queryCalls_;
    std::function<bool(const std::string&, std::string&)> queryHandler_;
};

// 被测对象
class UserRepository {
public:
    explicit UserRepository(Database& db) : db_(db) {}
    bool initialize(const std::string& url) { return db_.connect(url); }
    bool findUser(const std::string& name, std::string& out) {
        return db_.query("SELECT * FROM users WHERE name='" + name + "'", out);
    }
    void shutdown() { db_.disconnect(); }
private:
    Database& db_;
};
```

完整可运行示例见 [mock-objects 项目](../../cpp-mastery-roadmap/38-stage38/mock-objects)。

### 使用示例

```cpp
MockDatabase db;
UserRepository repo(db);

db.setShouldSucceed(true);
db.setLastQueryResult("Alice, alice@example.com");

bool ok = repo.initialize("localhost:5432");
assert(ok);
assert(db.connectCallCount() == 1);

std::string result;
repo.findUser("Alice", result);
assert(result == "Alice, alice@example.com");
assert(db.queryCallCount() == 1);
```

### Mock vs Stub vs Fake

| | Mock | Stub | Fake |
|---|---|---|---|
| 目标 | 验证交互 | 提供预设数据 | 轻量仿实现 |
| 行为 | 记录调用 | 返回固定值 | 有实际逻辑 |
| 典型场景 | "是否调用了 X" | "查不到时返回空" | 内存数据库 |

---

## 2. Test Fixture（测试夹具）

### 什么是 Fixture？

Fixture 把"每个测试都用到的初始化/清理代码"提取到一个共享结构中，避免重复 `setup()` / `teardown()` 逻辑，并保证测试间相互隔离。

> **核心思想**：`setup()` 准备数据，`teardown()` 清理状态，每个测试从干净状态开始。

### 代码实现

```cpp
#include <string>
#include <vector>
#include <algorithm>

struct TodoItem {
    int id;
    std::string text;
    bool done;
};

class TodoList {
public:
    int add(const std::string& text) {
        items_.push_back({nextId_++, text, false});
        return items_.back().id;
    }
    void complete(int id) {
        for (auto& item : items_)
            if (item.id == id) { item.done = true; return; }
    }
    void remove(int id) {
        items_.erase(
            std::remove_if(items_.begin(), items_.end(),
                [id](const TodoItem& i) { return i.id == id; }),
            items_.end());
    }
    std::vector<TodoItem> all() const { return items_; }
    std::size_t size() const { return items_.size(); }

private:
    std::vector<TodoItem> items_;
    int nextId_ = 1;
};

// 夹具：每个测试前自动 setup，测试后自动 teardown
class TodoTestFixture {
public:
    TodoTestFixture() { setup(); }
    ~TodoTestFixture() { teardown(); }

    void setup() {
        list_ = TodoList();
        id1_ = list_.add("Buy milk");
        id2_ = list_.add("Walk dog");
        id3_ = list_.add("Write code");
    }

    void teardown() {
        list_ = TodoList();  // 重置状态
    }

    TodoList& list() { return list_; }
    int id1() const { return id1_; }
    int id2() const { return id2_; }
    int id3() const { return id3_; }

private:
    TodoList list_;
    int id1_ = 0, id2_ = 0, id3_ = 0;
};
```

完整可运行示例见 [test-fixture 项目](../../cpp-mastery-roadmap/38-stage38/test-fixture)。

### 使用示例

```cpp
// 每个测试都从干净的 fixture 开始
void test_add_item() {
    TodoTestFixture f;
    int newId = f.list().add("New task");
    assert(f.list().size() == 4);  // 原有3条 + 新增1条
}

void test_complete_item() {
    TodoTestFixture f;
    f.list().complete(f.id1());    // 完成 "Buy milk"
    assert(!f.list().all()[0].done);  // 被 fixture 重置？不，这个是快照
    // 正确做法：fixture 重置后重新查
}
```

---

## 3. Parameterized Tests（参数化测试）

### 什么是参数化测试？

同一测试逻辑用不同数据反复运行——避免为每组数据写一个测试函数。数据驱动测试（DDT）的 C++ 实现。

### 代码实现

```cpp
#include <functional>
#include <string>
#include <vector>

struct TestCase {
    std::string name;
    std::function<bool()> test;
};

class TestRunner {
public:
    void addTest(const std::string& name, std::function<bool()> test) {
        tests_.push_back({name, std::move(test)});
    }

    struct Results {
        std::size_t total = 0, passed = 0, failed = 0;
        std::vector<std::string> failures;
    };

    Results run() const {
        Results r;
        r.total = tests_.size();
        for (const auto& t : tests_) {
            if (t.test()) { r.passed++; }
            else { r.failed++; r.failures.push_back(t.name); }
        }
        return r;
    }

private:
    std::vector<TestCase> tests_;
};
```

完整可运行示例见 [parameterized-tests 项目](../../cpp-mastery-roadmap/38-stage38/parameterized-tests)。

### 使用示例

```cpp
TestRunner runner;
runner.addTest("parse positive", [] { return parseInt("42").has_value(); });
runner.addTest("parse negative", [] { return parseInt("-7").has_value(); });
runner.addTest("parse zero", [] { return parseInt("0").has_value(); });
runner.addTest("parse invalid", [] { return !parseInt("abc").has_value(); });

auto r = runner.run();
assert(r.total == 4);
assert(r.passed == 4);
assert(r.failed == 0);
```

---

## 最佳实践

1. **Mock 验证行为，不验证实现**：Mock 关注"是否调用了正确方法"，而不是"用了哪个私有成员"
2. **Fake 不要有分支逻辑**：Fake 的目的是跑通路径，不是验证正确性——不要在 Fake 里写业务逻辑
3. **Fixture 要轻量**：每个测试都重新 setup，fixture 太重会拖慢测试套件
4. **参数化测试要覆盖边界**：空字符串、极大数、负数、特殊字符——数据要有代表性
5. **测试隔离是铁律**：测试 A 改了全局状态，不应影响测试 B 的结果

## 总结

三种测试模式各司其职：Mock 验证交互，Fixture 保证隔离，Parameterized Tests 批量验证等价路径。配合 CMake + CTest 自动化构建，是 C++ 项目的测试基础设施标准配置。
