# Task Scheduler API

## Overview

`task-scheduler` is a multithreaded task scheduler that supports a priority
queue, delayed execution and repeating tasks.

## Core Types

### TaskPriority

```cpp
enum class TaskPriority {
    Low = 0,    // low priority
    Normal = 1, // normal priority (the default)
    High = 2    // high priority
};
```

### TaskId

```cpp
using TaskId = std::size_t;
```

Every task gets a unique id.

## TaskScheduler Class API

### Construction

| Method | Signature | Description |
|---|---|---|
| Constructor | `TaskScheduler(std::size_t threadCount = 4)` | Creates a scheduler with 4 worker threads by default |

The worker threads are created by the constructor, so tasks can be scheduled
before `start()` is called. `start()` only flips the running flags.

### Task Scheduling

| Method | Signature | Description |
|---|---|---|
| `schedule` | `TaskId schedule(TaskFunc func, TaskPriority priority = TaskPriority::Normal)` | Schedules a task to run immediately |
| `scheduleAt` | `TaskId scheduleAt(TaskFunc func, std::chrono::steady_clock::time_point time, TaskPriority priority = TaskPriority::Normal)` | Schedules a task to run later |
| `scheduleRepeating` | `TaskId scheduleRepeating(TaskFunc func, std::chrono::milliseconds interval, TaskPriority priority = TaskPriority::Normal)` | Schedules a task to run on a fixed interval |
| `cancel` | `void cancel(TaskId id)` | Cancels a task if it is still queued |

### Lifecycle

| Method | Signature | Description |
|---|---|---|
| `start` | `void start()` | Marks the scheduler as running |
| `stop` | `void stop()` | Stops the scheduler and waits for the workers to finish |
| `pendingTasks` | `std::size_t pendingTasks() const` | Returns the number of tasks waiting in the queue |
| `completedTasks` | `std::size_t completedTasks() const` | Returns the number of tasks that have run |

### Thread Safety

- Every public method is safe to call from multiple threads.
- The priority queue is protected by a `std::mutex` plus a
  `std::condition_variable`.
- `stopped_` is a `std::atomic<bool>`, so the worker loop can check it
  without taking the lock.

## Usage Example

```cpp
#include "task_scheduler.hpp"
#include <chrono>
#include <iostream>
#include <thread>

using namespace task_scheduler;
using Clock = std::chrono::steady_clock;

int main() {
    TaskScheduler scheduler(4);

    // A normal task.
    auto id1 = scheduler.schedule([]() {
        std::cout << "Task 1 executed\n";
    });

    // A delayed task.
    auto id2 = scheduler.scheduleAt([]() {
        std::cout << "Delayed task executed\n";
    }, Clock::now() + std::chrono::seconds(1));

    // A repeating task.
    auto id3 = scheduler.scheduleRepeating([]() {
        std::cout << "Repeating task\n";
    }, std::chrono::milliseconds(500));

    scheduler.start();
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Cancel the tasks that are still queued.
    scheduler.cancel(id1);
    scheduler.cancel(id2);
    scheduler.cancel(id3);

    scheduler.stop();
    std::cout << "Completed: " << scheduler.completedTasks() << " tasks\n";
    return 0;
}
```

## Exception Safety and Cancellation Semantics

| Scenario | Behavior |
|---|---|
| The task throws | The worker catches it and prints it to stderr (`task <id> threw: <message>`); the worker keeps running and the task still counts towards `completedTasks` |
| `cancel(id)` before the task is dequeued | The task never runs and does not count towards `completedTasks` |
| `cancel(id)` after the task is dequeued | No effect: the task is already running or finished, and the `activeTasks_` flag was consumed when it was dequeued |
| Cancelling a repeating task | Stops the later ticks; a tick that was already queued for the next round can still be skipped |
| What a task body must avoid | Blocking forever (for example a `sleep_for` that never returns) occupies a worker until `stop()` times out waiting for it |

## Usage Example (with assertions)

See [`examples/task-scheduler-error-handling.cpp`](../../examples/task-scheduler-error-handling.cpp):

```cpp
TaskScheduler scheduler(1);
std::atomic<int> delayedRuns{0};
auto delayedId = scheduler.scheduleAt([&]() { delayedRuns++; },
                                       Clock::now() + std::chrono::milliseconds(300));
scheduler.start();
scheduler.cancel(delayedId);               // cancel the delayed task before it is due
std::this_thread::sleep_for(std::chrono::milliseconds(600));
scheduler.stop();
assert(delayedRuns == 0);                  // a cancelled task must not run
```

## Best Practices

1. **Choosing the thread count**: match the number of worker threads to the
   core count; IO-bound work can justify more.
2. **Task granularity**: avoid long blocking operations inside a scheduler
   thread.
3. **Exception handling**: exceptions are caught by the worker and printed to
   stderr instead of killing the worker, but production code should catch
   inside the task body and record structured logs.
4. **Graceful shutdown**: cancel the tasks that have not run yet before
   calling `stop()`.
