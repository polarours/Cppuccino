# key-value-store

一个用现代 C++ 编写的小型有状态存储练习。

本项目是中级路线图的一部分，旨在以比 HTTP 服务器项目更直接的方式
教授状态所有权、命令处理和持久化边界。

## 学习目标

- 显式表示长生命周期状态
- 保持命令解析与存储行为分离
- 使持久化决策可见而非隐式
- 为格式错误的命令和缺失的键定义简单的失败行为
- 构建一个有足够状态的小型系统以需要测试

## 非目标

- 构建生产级数据库
- 支持复杂的查询语言或事务

## 功能

- **命令**: `SET`, `GET`, `DELETE`, `EXISTS`, `COUNT`, `CLEAR`, `SAVE`, `LOAD`, `EXIT`
- **协议**: 简单的文本协议，前缀格式
- **持久化**: 基于文件的存储
- **并发**: 共享互斥锁保护
- **服务器**: 多客户端 TCP 服务器

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
# 服务器模式
./build/kv_server

# CLI 模式
./build/kv_server --cli
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
