# 简单数据库

一个内存数据库，支持表管理和文件持久化。

## 学习目标

- 理解数据建模和存储
- 练习变体类型（std::variant）的使用
- 学习文件持久化模式
- 实现 CRUD 操作

## 非目标

- 构建生产级数据库
- 实现 SQL 解析器
- 支持并发访问

## 功能

- 创建/删除表
- 插入、查询、更新、删除行
- 使用谓词过滤
- 保存/加载到文件
- 多种数据类型（int、double、string、bool、null）

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/simple_db_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "simple_db.hpp"

simple_db::Database db;
db.createTable("users");
db.insert("users", {{"name", "Alice"}, {"age", 25}});

auto users = db.select("users");
db.save("database.txt");
```

## 建议的下一步

- 添加类似 SQL 的查询解析器
- 实现索引以加快查找
- 添加事务支持
- 实现 WHERE 子句解析
