# 第四阶段：构建更大的系统

本阶段的目标是将语义知识转化为实际代码中的设计决策。

## 学习目标

- 理解大型项目的设计权衡
- 实践模块化和接口设计
- 学习错误处理和资源管理
- 在真实场景中应用 C++ 特性

## 项目概述

### simple-db（简单数据库）

一个内存数据库，支持表管理和文件持久化。

**学习重点：**
- 数据建模和存储设计
- 变体类型（std::variant）的使用
- 文件 I/O 和序列化
- CRUD 操作实现

**功能：**
- 创建/删除表
- 插入、查询、更新、删除行
- 支持多种数据类型（int, double, string, bool, null）
- 保存/加载到文件

**构建和运行：**
```bash
cd simple-db
cmake -S . -B build
cmake --build build
./build/simple_db_example
```

### chat-server（聊天服务器）

一个支持多用户的聊天服务器，支持房间和命令。

**学习重点：**
- 网络编程和套接字
- 多线程并发处理
- 消息广播和路由
- 用户管理和房间系统

**功能：**
- 多用户 TCP 聊天
- 房间管理
- 用户加入/离开通知
- 命令系统：/join, /leave, /rooms, /users, /quit

**构建和运行：**
```bash
cd chat-server
cmake -S . -B build
cmake --build build
./build/chat_server_example
```

### web-server（Web 服务器）

一个支持路由和中间件的 HTTP 服务器。

**学习重点：**
- HTTP 协议和请求/响应
- 套接字编程和并发
- 路由和中间件模式
- 静态文件服务

**功能：**
- HTTP 请求解析
- 路由注册（GET, POST, PUT, DELETE）
- 中间件支持
- 静态文件服务
- 基于线程的并发

**构建和运行：**
```bash
cd web-server
cmake -S . -B build
cmake --build build
./build/web_server_example
```

## 建议的学习路径

1. **选择一个项目**开始
2. **阅读代码**理解当前实现
3. **识别设计决策**：所有权边界、生命周期假设、错误路径
4. **添加一个功能**：更好的错误处理、更多测试、文档
5. **重构代码**使其更清晰、更可维护

## 非目标

- 构建生产级系统
- 实现完整的功能集
- 追求最大性能

这些项目是学习工具，不是最终产品。
