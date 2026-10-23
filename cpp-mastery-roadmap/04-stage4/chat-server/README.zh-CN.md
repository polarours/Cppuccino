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

## 关闭流程

`stop()` 必须能叫醒一个正卡在 `accept()` 或 `recv()` 里的线程。在 Linux
上，这两者都不会因为某个标志位被置上就醒过来，所以要关的是套接字本身。

让这件事安全的，是归属关系：

- 监听套接字由 `start()` 创建，也由 `start()` 在 accept 循环结束时关闭。
  `stop()` 只负责 shutdown，并且是在持锁的情况下做的。一个描述符如果被别人
  关掉了，而另一个线程还留着它的编号，那个编号说不定已经归别人所有。
- 恰好在服务器要停的那一刻被 accept 进来的连接不会拿到线程。运行标志和
  线程列表是在同一把锁下读的，所以这种连接会被直接关掉，而不是落进一个
  没人 join 的列表里。
- 客户端线程是在持锁的情况下从列表里取出来、在锁外 join 的 —— 因为客户端
  线程广播告别消息时自己也要那把锁。

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
