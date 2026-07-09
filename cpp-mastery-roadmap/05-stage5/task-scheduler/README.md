# Task Scheduler

A priority-based task scheduler with support for delayed and repeating tasks.

## Learning Goals

- Understand priority queue data structures
- Practice thread synchronization and producer-consumer patterns
- Learn task scheduling algorithms
- Implement work distribution across threads

## Non-Goals

- Building a production job scheduler
- Implementing distributed task execution
- Supporting task dependencies

## Features

- Priority-based task scheduling
- Immediate, delayed, and repeating tasks
- Task cancellation
- Multi-threaded execution
- Thread-safe operation

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/task_scheduler_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "task_scheduler.hpp"

task_scheduler::TaskScheduler scheduler(4);
scheduler.start();

// Immediate task
scheduler.schedule([]() {
    std::cout << "Executed!\n";
});

// High priority task
scheduler.schedule([]() {
    std::cout << "High priority!\n";
}, task_scheduler::TaskPriority::High);

// Repeating task
auto id = scheduler.scheduleRepeating([]() {
    std::cout << "Repeating!\n";
}, std::chrono::milliseconds(100));

// Cancel
scheduler.cancel(id);

scheduler.stop();
```

## Suggested Next Steps

- Add task dependencies and DAG execution
- Implement task timeout and retry
- Add task progress tracking
- Implement distributed task scheduling
