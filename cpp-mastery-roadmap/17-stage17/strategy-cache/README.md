# Strategy Cache

A cache with configurable strategies (LRU, TTL).

## Learning Goals

- Understand cache strategy patterns
- Practice strategy pattern in caching
- Learn LRU and TTL strategies
- Implement strategy switching

## Non-Goals

- Building a production cache
- Implementing complex eviction policies
- Supporting distributed caching

## Features

- CacheStrategy interface
- LRU strategy with capacity
- TTL strategy with expiration
- Cache context for strategy switching
- Thread-safe operations

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/strategy_cache_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "strategy_cache.hpp"

strategy_cache::Cache<std::string, int> cache;
cache.setStrategy(std::make_shared<strategy_cache::LRUStrategy<std::string, int>>(100));

cache.put("key", 42);
auto val = cache.get("key");
```

## Suggested Next Steps

- Add more caching strategies
- Implement strategy composition
- Add cache statistics
- Implement a distributed cache
