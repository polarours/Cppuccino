# Ring Buffer

A thread-safe circular buffer implementation.

## Learning Goals

- Understand circular buffer data structures
- Practice thread synchronization for producer-consumer patterns
- Learn lock-free and lock-based implementations
- Implement blocking and non-blocking operations

## Non-Goals

- Building a production ring buffer
- Implementing lock-free algorithms
- Supporting multiple producers/consumers

## Features

- Thread-safe push/pop operations
- Blocking pop with timeout
- Peek without removal
- FIFO ordering
- Configurable capacity

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/ring_buffer_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "ring_buffer.hpp"

ring_buffer::RingBuffer<int> buffer(10);

// Producer
buffer.push(42);

// Consumer
auto item = buffer.pop();
if (item) {
    std::cout << "Got: " << *item << "\n";
}

// Blocking pop
auto item2 = buffer.popWait(std::chrono::seconds(1));
```

## Suggested Next Steps

- Implement lock-free version
- Add multiple producer/consumer support
- Add priority support
- Implement batch operations
