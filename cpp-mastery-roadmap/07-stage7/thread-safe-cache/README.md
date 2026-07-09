# Thread-Safe Cache

A generic thread-safe cache with TTL support and eviction policies.

## Learning Goals

- Understand caching strategies and data structures
- Practice read-write locks for concurrent access
- Learn TTL-based expiration
- Implement LRU eviction

## Non-Goals

- Building a production cache like Redis
- Implementing distributed caching
- Supporting complex eviction policies

## Features

- Generic template-based cache
- Thread-safe with read-write locks
- TTL-based expiration
- LRU eviction when at capacity
- getOrElse and getOrCreate helpers

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/thread_safe_cache_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "thread_safe_cache.hpp"

thread_safe_cache::Cache<std::string, int> cache(1000);

// Set with TTL
cache.set("session", 12345, std::chrono::minutes(30));

// Get with default
int value = cache.getOrElse("missing", 0);

// Get or create
int count = cache.getOrCreate("counter", []() { return 0; });
```

## Suggested Next Steps

- Add hit/miss statistics
- Implement different eviction policies (LFU, FIFO)
- Add cache warming strategies
- Implement distributed cache protocol
