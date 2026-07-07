# Event Loop

A simple event loop implementation for asynchronous programming.

## Learning Goals

- Understand event-driven programming
- Practice callback-based async patterns
- Learn timer scheduling and cancellation
- Implement task queue with priority scheduling

## Non-Goals

- Building a production-grade event loop
- Implementing I/O multiplexing (epoll, kqueue)

## Features

- Post immediate tasks
- Schedule delayed tasks with timers
- Repeating timer support
- Timer cancellation
- Graceful shutdown

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/event_loop_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "event_loop.hpp"

event_loop::EventLoop loop;

// Immediate task
loop.post([]() {
    std::cout << "Immediate!\n";
});

// Delayed task
loop.schedule([]() {
    std::cout << "Delayed!\n";
}, std::chrono::milliseconds(100));

// Repeating task
auto id = loop.scheduleRepeating([]() {
    std::cout << "Repeating!\n";
}, std::chrono::milliseconds(50));

// Cancel timer
loop.cancelTimer(id);

// Run
loop.run();
```

## Suggested Next Steps

- Add I/O event handling (file descriptors, sockets)
- Implement event multiplexing (epoll/kqueue)
- Add coroutine integration
- Add event prioritization
