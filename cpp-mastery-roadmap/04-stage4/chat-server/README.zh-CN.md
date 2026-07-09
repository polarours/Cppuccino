# 聊天服务器

一个支持多用户的聊天服务器，支持房间和命令。

## 学习目标

- 理解网络编程和套接字
- 练习多线程并发处理
- 学习消息广播和路由
- 实现用户管理和房间系统

## 非目标

- 构建生产级聊天服务器
- 实现加密或认证
- 支持 WebSocket 或 HTTP

## 功能

- 多用户 TCP 聊天
- 房间管理
- 用户加入/离开通知
- 命令系统：/join、/leave、/rooms、/users、/quit
- 线程安全操作

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/chat_server_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```bash
# 终端 1
./build/chat_server_example

# 终端 2
telnet localhost 8080
> Alice
> Hello everyone!
> /join general
> /users
```

## 建议的下一步

- 添加私聊功能
- 实现消息历史
- 添加文件共享支持
- 实现用户认证
