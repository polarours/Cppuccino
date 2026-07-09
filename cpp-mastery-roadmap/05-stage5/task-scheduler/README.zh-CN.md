# 任务调度器

一个基于优先级的任务调度器，支持延迟和重复任务。

## 学习目标

- 理解优先队列数据结构
- 练习线程同步和生产者-消费者模式
- 学习任务调度算法
- 实现跨线程的工作分配

## 非目标

- 构建生产级作业调度器
- 实现分布式任务执行
- 支持任务依赖

## 功能

- 基于优先级的任务调度
- 即时、延迟和重复任务
- 任务取消
- 多线程执行
- 线程安全操作

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/task_scheduler_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "task_scheduler.hpp"

task_scheduler::TaskScheduler scheduler(4);
scheduler.start();

// 即时任务
scheduler.schedule([]() {
    std::cout << "Executed!\n";
});

// 高优先级任务
scheduler.schedule([]() {
    std::cout << "High priority!\n";
}, task_scheduler::TaskPriority::High);

// 重复任务
auto id = scheduler.scheduleRepeating([]() {
    std::cout << "Repeating!\n";
}, std::chrono::milliseconds(100));

// 取消
scheduler.cancel(id);

scheduler.stop();
```

## 建议的下一步

- 添加任务依赖和 DAG 执行
- 实现任务超时和重试
- 添加任务进度跟踪
- 实现分布式任务调度
