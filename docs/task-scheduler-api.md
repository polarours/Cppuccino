# Task Scheduler API 文档

## 概述

`task-scheduler` 是一个多线程任务调度器，支持优先级队列、延迟执行和周期性任务。

## 核心类型

### TaskPriority

```cpp
enum class TaskPriority {
    Low = 0,    // 低优先级
    Normal = 1, // 普通优先级（默认）
    High = 2    // 高优先级
};
```

### TaskId

```cpp
using TaskId = std::size_t;
```

每个任务分配唯一 ID。

## TaskScheduler 类 API

### 构造

| 方法 | 签名 | 说明 |
|---|---|---|
| 构造器 | `TaskScheduler(std::size_t threadCount = 4)` | 创建调度器，默认 4 个工作线程 |

### 任务调度

| 方法 | 签名 | 说明 |
|---|---|---|
| `schedule` | `TaskId schedule(TaskFunc func, TaskPriority priority = TaskPriority::Normal)` | 立即调度任务 |
| `scheduleAt` | `TaskId scheduleAt(TaskFunc func, std::chrono::steady_clock::time_point time, TaskPriority priority = TaskPriority::Normal)` | 延迟执行 |
| `scheduleRepeating` | `TaskId scheduleRepeating(TaskFunc func, std::chrono::milliseconds interval, TaskPriority priority = TaskPriority::Normal)` | 周期性执行 |
| `cancel` | `void cancel(TaskId id)` | 取消任务（如果在队列中） |

### 生命周期

| 方法 | 签名 | 说明 |
|---|---|---|
| `start` | `void start()` | 启动工作线程池 |
| `stop` | `void stop()` | 停止调度器，等待所有任务完成 |
| `pendingTasks` | `std::size_t pendingTasks() const` | 返回等待执行的任务数 |
| `completedTasks` | `std::size_t completedTasks() const` | 返回已完成的任务数 |

### 线程安全

- 所有公共方法对多个调用者线程是安全的
- 内部使用 `std::mutex` + `std::condition_variable` 保护优先级队列
- `stopped_` 是 `std::atomic<bool>`，用于无锁检查

## 使用示例

```cpp
#include "task_scheduler.hpp"
#include <iostream>
#include <chrono>

using namespace task_scheduler;
using Clock = std::chrono::steady_clock;

int main() {
    TaskScheduler scheduler(4);

    // 普通任务
    auto id1 = scheduler.schedule([]() {
        std::cout << "Task 1 executed\n";
    });

    // 延迟任务
    auto id2 = scheduler.scheduleAt([]() {
        std::cout << "Delayed task executed\n";
    }, Clock::now() + std::chrono::seconds(1));

    // 周期性任务
    auto id3 = scheduler.scheduleRepeating([]() {
        std::cout << "Repeating task\n";
    }, std::chrono::milliseconds(500));

    scheduler.start();
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 取消任务
    scheduler.cancel(id2);

    scheduler.stop();
    std::cout << "Completed: " << scheduler.completedTasks() << " tasks\n";
    return 0;
}
```

## 最佳实践

1. **线程数选择**：工作线程数应匹配 CPU 核心数，IO 密集型可适当增加
2. **任务粒度**：避免在调度器线程中执行长时间阻塞操作
3. **异常处理**：任务内异常不应传播到调度器，建议 catch 后记录日志
4. **Graceful Shutdown**：先 cancel 未执行任务，再调用 stop()