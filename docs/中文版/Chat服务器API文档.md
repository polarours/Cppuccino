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

- 共享状态由 `std::mutex` 保护；公共查询和发送方法在读写前后都会加锁。
- `clientThreads_` 持有每个客户端的连接线程。`stop()` 会 join 每一个线程
  才返回，因此该 vector 不会在客户端仍在运行时被并发修改。
- 消息广播和房间操作对其他调用者而言是原子的，因为整个操作期间都持有锁。

## 完整示例

```cpp
#include "chat_server.hpp"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

// 连接服务器，发送若干行并收集回复。
std::vector<std::string> talk(int port, const std::string& user,
                              const std::vector<std::string>& lines) {
    int fd = -1;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // start() 在自己的线程上完成 bind 和 listen，因此需要重试，
    // 直到监听套接字真正开始接受连接。
    for (int attempt = 0; attempt < 50; ++attempt) {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            return {};
        }
        if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
            break;
        }
        close(fd);
        fd = -1;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if (fd < 0) {
        return {};
    }

    auto sendLine = [&](const std::string& text) {
        send(fd, (text + "\n").c_str(), text.size() + 1, 0);
    };
    sendLine(user);  // 第一行是用户名

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    for (const auto& line : lines) {
        sendLine(line);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::string buffer(4096, '\0');
    std::vector<std::string> replies;
    ssize_t bytes = recv(fd, buffer.data(), buffer.size() - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::string reply(buffer.data());
        size_t start = 0;
        while (start < reply.size()) {
            size_t end = reply.find('\n', start);
            if (end == std::string::npos) end = reply.size();
            if (end > start) replies.push_back(reply.substr(start, end - start));
            start = end + 1;
        }
    }
    close(fd);
    return replies;
}

int main(int argc, char** argv) {
    // 端口可配置，因为 8080 经常已被占用
    const int port = argc > 1 ? std::atoi(argv[1]) : 8080;

    chat_server::ChatServer server(port);

    // 设置事件回调
    server.setOnUserJoin([](const std::string& user) {
        std::cout << user << " 加入了服务器\n";
    });
    server.setOnUserLeave([](const std::string& user) {
        std::cout << user << " 离开了服务器\n";
    });

    // start() 会阻塞在 accept 循环中，因此放到独立线程上运行
    std::thread serverThread([&server]() {
        server.start();
    });

    // 像真实客户端那样驱动命令接口
    for (const auto& reply : talk(port, "Alice", {"/join general", "/rooms"})) {
        std::cout << reply << "\n";
    }

    server.stop();
    if (serverThread.joinable()) {
        serverThread.join();
    }
    return 0;
}
```

## 构建与测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```