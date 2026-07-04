# Thread Pool

A simple, modern C++ thread pool implementation.

## Learning Goals

- Understand thread synchronization with mutex and condition variables
- Practice move-only types and std::future/std::packaged_task
- Learn producer-consumer pattern
- Apply RAII to thread management

## Non-Goals

- Building a production-grade thread pool
- Implementing work-stealing or task prioritization
- Supporting cancellation or exception propagation

## Features

- Fixed-size thread pool
- Submit tasks with `enqueue()` and get `std::future`
- `waitForAll()` to block until all tasks complete
- `shutdown()` for graceful termination
- Thread-safe by design

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/thread_pool_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "thread_pool.hpp"

thread_pool::ThreadPool pool(4);

// Enqueue a task
auto future = pool.enqueue([]() {
    return 42;
});

// Get result
int result = future.get();  // 42

// Task with arguments
auto sum = pool.enqueue([](int a, int b) {
    return a + b;
}, 3, 4);

// Wait for all tasks
pool.waitForAll();
```

## Suggested Next Steps

- Add task cancellation support
- Implement work-stealing for better load balancing
- Add task priorities
- Add dynamic thread pool resizing
