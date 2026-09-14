# Chat Server API 文档

## 概述

`chat-server` 是一个多线程 TCP 聊天服务器，支持房间管理、消息广播和用户命令。

## 核心类型

### Message

```cpp
struct Message {
    std::string sender;   // 发送者用户名
    std::string content;  // 消息内容
    std::string room;     // 目标房间
    std::string timestamp; // 时间戳
};
```

## ChatServer 类 API

### 构造与生命周期

| 方法 | 签名 | 说明 |
|---|---|---|
| 构造器 | `ChatServer(int port)` | 绑定到指定端口 |
| `start` | `void start()` | 启动服务器线程 |
| `stop` | `void stop()` | 停止服务器，等待所有客户端线程结束 |
| `isRunning` | `bool isRunning() const` | 检查服务器是否运行中 |

### 房间管理

| 方法 | 签名 | 说明 |
|---|---|---|
| `joinRoom` | `void joinRoom(const std::string& user, const std::string& room)` | 用户加入房间 |
| `leaveRoom` | `void leaveRoom(const std::string& user, const std::string& room)` | 用户离开房间 |
| `getRooms` | `std::vector<std::string> getRooms() const` | 获取所有房间列表 |
| `getRoomUsers` | `std::vector<std::string> getRoomUsers(const std::string& room) const` | 获取房间内用户列表 |
| `getOnlineUsers` | `std::vector<std::string> getOnlineUsers() const` | 获取所有在线用户 |

### 消息发送

| 方法 | 签名 | 说明 |
|---|---|---|
| `broadcast` | `void broadcast(const Message& msg)` | 向所有用户广播消息 |
| `sendToUser` | `void sendToUser(const std::string& user, const Message& msg)` | 发送给指定用户 |
| `sendToRoom` | `void sendToRoom(const std::string& room, const Message& msg)` | 发送给房间内所有用户 |

### 事件回调

| 方法 | 签名 | 说明 |
|---|---|---|
| `setOnMessage` | `void setOnMessage(std::function<void(const Message&)>)` | 设置消息回调 |
| `setOnUserJoin` | `void setOnUserJoin(std::function<void(const std::string&)>)` | 设置用户加入回调 |
| `setOnUserLeave` | `void setOnUserLeave(std::function<void(const std::string&)>)` | 设置用户离开回调 |

### 内部处理（测试用）

| 方法 | 签名 | 说明 |
|---|---|---|
| `processCommand` | `void processCommand(const std::string& user, const std::string& cmd)` | 处理用户命令 |

## 支持的命令

| 命令 | 格式 | 说明 |
|---|---|---|
| `/join` | `/join <room>` | 加入房间 |
| `/leave` | `/leave <room>` | 离开房间 |
| `/rooms` | `/rooms` | 列出所有房间 |
| `/users` | `/users` | 列出当前用户 |
| `/quit` | `/quit` | 退出服务器 |

## 线程安全

- 所有公共方法通过 `std::mutex` 保护共享状态
- `clientThreads_` 管理每个客户端的连接线程
- 消息广播和房间操作都是原子性的

## 完整示例

```cpp
#include "chat_server.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    chat_server::ChatServer server(8080);

    // 设置事件回调
    server.setOnUserJoin([](const std::string& user) {
        std::cout << user << " 加入了服务器\n";
    });
    server.setOnUserLeave([](const std::string& user) {
        std::cout << user << " 离开了服务器\n";
    });

    server.start();

    // 模拟用户操作
    server.processCommand("Alice", "/join general");
    server.processCommand("Bob", "/join general");

    chat_server::Message msg{"Alice", "Hello everyone!", "general", ""};
    server.sendToRoom("general", msg);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    server.stop();

    return 0;
}
```

## 构建与测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```