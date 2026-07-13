# Command Queue

A thread-safe command queue for async task execution.

## Learning Goals

- Understand producer-consumer pattern
- Practice thread synchronization
- Learn task queue implementation
- Implement concurrent execution

## Non-Goals

- Building a production task queue
- Implementing complex scheduling
- Supporting distributed queues

## Features

- Thread-safe task queue
- Multiple worker threads
- Wait for all completion
- Graceful shutdown
- Task count tracking

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/command_queue_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "command_queue.hpp"

command_queue::CommandQueue queue(4);
queue.enqueue([]() { /* work */ });
queue.waitForAll();
```

## 建议的下一步

- 添加任务优先级
- 实现任务取消
- 添加任务超时
- 实现分布式队列
