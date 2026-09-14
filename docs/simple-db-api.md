# Simple Database API 文档

## 概述

`simple-db` 是一个内存数据库，支持表管理、CRUD 操作和文件持久化。核心数据结构基于 `std::variant` 实现多类型值。

## 核心类型

### Value

```cpp
using Value = std::variant<int, double, std::string, bool, std::nullptr_t>;
```

支持五种类型：整数、浮点数、字符串、布尔值、空值。

### Row / Rows

```cpp
using Row = std::map<std::string, Value>;
using Rows = std::vector<Row>;
```

- `Row`：一行数据，key 为列名，value 为单元格值
- `Rows`：多行数据

## Database 类 API

### 构造与表管理

| 方法 | 签名 | 说明 |
|---|---|---|
| `createTable` | `bool createTable(const std::string& name)` | 创建表，已存在返回 false |
| `dropTable` | `bool dropTable(const std::string& name)` | 删除表，不存在返回 false |
| `tableExists` | `bool tableExists(const std::string& name) const` | 检查表是否存在 |

### CRUD 操作

| 方法 | 签名 | 说明 |
|---|---|---|
| `insert` | `bool insert(const std::string& table, const Row& row)` | 插入一行，表不存在返回 false |
| `select` | `Rows select(const std::string& table, const std::string& where = "", const std::function<bool(const Row&)>& predicate = nullptr)` | 查询所有行，支持谓词过滤 |
| `update` | `bool update(const std::string& table, const std::map<std::string, Value>& updates, const std::function<bool(const Row&)>& predicate = nullptr)` | 更新匹配的行，返回是否成功 |
| `remove` | `bool remove(const std::string& table, const std::function<bool(const Row&)>& predicate = nullptr)` | 删除匹配的行，返回是否删除了行 |

### 工具方法

| 方法 | 签名 | 说明 |
|---|---|---|
| `count` | `std::size_t count(const std::string& table) const` | 返回表中的行数 |
| `clear` | `void clear(const std::string& table)` | 清空表数据（保留表结构） |
| `tables` | `const std::map<std::string, Rows>& tables() const` | 返回所有表和数据的引用 |

### 持久化

| 方法 | 签名 | 说明 |
|---|---|---|
| `save` | `bool save(const std::string& filename) const` | 保存到文件（格式：`TABLE:name` + `ROW|key=value`） |
| `load` | `bool load(const std::string& filename)` | 从文件加载，清空现有数据 |

## 辅助函数

```cpp
std::string valueToString(const Value& value);
Value stringToValue(const std::string& str);
```

将 Value 序列化和反序列化为字符串，用于文件存储。

## 完整示例

```cpp
#include "simple_db.hpp"
#include <iostream>

int main() {
    simple_db::Database db;

    // 创建表
    db.createTable("users");

    // 插入数据
    db.insert("users", {{"name", "Alice"}, {"age", 30}});
    db.insert("users", {{"name", "Bob"}, {"age", 25}});
    db.insert("users", {{"name", "Charlie"}, {"age", 35}});

    // 查询
    auto all = db.select("users");
    std::cout << "Total users: " << all.size() << "\n";

    // 谓词过滤
    auto adults = db.select("users", "", [](const simple_db::Row& row) {
        auto it = row.find("age");
        if (it != row.end() && std::holds_alternative<int>(it->second)) {
            return std::get<int>(it->second) >= 18;
        }
        return false;
    });
    std::cout << "Adults: " << adults.size() << "\n";

    // 更新
    db.update("users", {{"age", 31}}, [](const simple_db::Row& row) {
        auto it = row.find("name");
        return it != row.end() && std::get<std::string>(it->second) == "Alice";
    });

    // 删除
    db.remove("users", [](const simple_db::Row& row) {
        auto it = row.find("name");
        return it != row.end() && std::get<std::string>(it->second) == "Bob";
    });

    // 持久化
    db.save("database.txt");

    return 0;
}
```

## 构建与测试

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```