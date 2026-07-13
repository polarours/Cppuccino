# 命令队列

一个线程安全的命令队列，用于异步任务执行。

## 学习目标

- 理解生产者-消费者模式
- 练习线程同步
- 学习任务队列实现
- 实现并发执行

## 非目标

- 构建生产级任务队列
- 实现复杂的调度
- 支持分布式队列

## 功能

- 线程安全的任务队列
- 多个工作线程
- 等待所有完成
- 优雅关闭
- 任务计数跟踪

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/command_queue_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "command_queue.hpp"

command_queue::CommandQueue queue(4);
queue.enqueue([]() { /* 工作 */ });
queue.waitForAll();
```

## 建议的下一步

- 添加任务优先级
- 实现任务取消
- 添加任务超时
- 实现分布式队列
