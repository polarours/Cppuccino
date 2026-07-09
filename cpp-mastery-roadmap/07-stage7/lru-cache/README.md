# LRU Cache

A thread-safe Least Recently Used (LRU) cache implementation.

## Learning Goals

- Understand LRU eviction policy
- Practice thread synchronization for cache operations
- Learn cache data structures (hash map + doubly linked list)
- Implement cache hit/miss patterns

## Non-Goals

- Building a production cache
- Implementing distributed caching
- Supporting complex eviction policies

## Features

- O(1) get and put operations
- LRU eviction when at capacity
- Thread-safe with mutex
- Contains and size queries
- Clear operation

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/lru_cache_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "lru_cache.hpp"

lru_cache::LRUCache<std::string, int> cache(100);

cache.put("key1", 42);
auto value = cache.get("key1");  // 42

cache.put("key2", 100);
cache.get("key1");  // key1 is now most recently used

// Adding new entry evicts key2 (least recently used)
cache.put("key3", 200);
```

## Suggested Next Steps

- Add hit/miss statistics
- Implement TTL support
- Add cache warming strategies
- Implement distributed LRU cache
