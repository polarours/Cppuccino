# Producer-Consumer Demo

This project demonstrates the classic **producer-consumer pattern** using the thread-safe `ring_buffer::RingBuffer` from the Stage 6 ring-buffer project.

## Learning Goals

- Understand the producer-consumer concurrency pattern
- Learn how to use a shared buffer with thread-safe push/pop operations
- Practice using condition variables (internally implemented in RingBuffer)
- Demonstrate backpressure handling when the buffer is full

## Project Structure

- `main.cpp`: Demo program with multiple producers and consumers
- `tests/pcon_tests.cpp`: Unit tests for thread-safe ring buffer usage
- `ring_buffer.hpp`: Included from the existing ring-buffer project

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/producer_consumer_demo
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Related Projects

- [ring-buffer](../ring-buffer/): Thread-safe circular buffer implementation
- [connection-pool](../connection-pool/): Another concurrency pattern using shared resources
- [guarded-suspension](../../03-advanced/guarded-suspension/): Wait for condition before proceeding

## Suggested Extensions

- Add a shutdown signal to gracefully stop consumers
- Implement a bounded producer with backpressure feedback
- Add statistics tracking (throughput, latency)
