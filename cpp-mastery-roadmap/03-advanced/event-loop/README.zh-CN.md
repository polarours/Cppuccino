# 事件循环

一个简单的事件循环实现，用于异步编程。

## 学习目标

- 理解事件驱动编程
- 练习基于回调的异步模式
- 学习定时器调度和取消
- 实现带优先级调度的任务队列

## 非目标

- 构建生产级事件循环
- 实现 I/O 多路复用（epoll、kqueue）

## 功能

- 发布即时任务
- 使用定时器调度延迟任务
- 重复定时器支持
- 定时器取消
- 优雅关闭

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/event_loop_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "event_loop.hpp"

event_loop::EventLoop loop;

// 即时任务
loop.post([]() {
    std::cout << "Immediate!\n";
});

// 延迟任务
loop.schedule([]() {
    std::cout << "Delayed!\n";
}, std::chrono::milliseconds(100));

// 重复任务
auto id = loop.scheduleRepeating([]() {
    std::cout << "Repeating!\n";
}, std::chrono::milliseconds(50));

// 取消定时器
loop.cancelTimer(id);

// 运行
loop.run();
```

## 建议的下一步

- 添加 I/O 事件处理（文件描述符、套接字）
- 实现事件多路复用（epoll/kqueue）
- 添加协程集成
- 添加事件优先级
