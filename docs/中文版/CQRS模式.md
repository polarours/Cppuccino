# CQRS 模式 (CQRS Pattern)

## 什么是 CQRS 模式？

CQRS（Command Query Responsibility Segregation）模式将读操作和写操作分离到不同的模型中。

## 核心概念

- **命令（Command）**：改变状态的操作
- **查询（Query）**：读取数据不改变状态
- **命令处理器**：处理写操作
- **查询处理器**：处理读操作

## 使用场景

1. 读写比例悬殊
2. 需要独立扩展读写能力
3. 需要事件审计

## 与 Event Sourcing 的关系

CQRS 常与事件溯源配合使用，但两者独立。

