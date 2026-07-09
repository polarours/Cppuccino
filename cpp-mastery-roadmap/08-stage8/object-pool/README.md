# Object Pool

A generic object pool for reusing expensive objects.

## Learning Goals

- Understand object reuse patterns
- Practice shared_ptr with custom deleters
- Learn pool management and lifecycle
- Implement thread-safe object acquisition

## Non-Goals

- Building a production object pool
- Implementing connection-specific features
- Supporting complex eviction policies

## Features

- Generic template-based pool
- Shared pointer with custom deleter for automatic return
- Thread-safe acquire/release
- Configurable initial pool size
- Clear operation

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/object_pool_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "object_pool.hpp"

object_pool::ObjectPool<Connection> pool(
    []() { return new Connection(); },
    [](Connection* p) { delete p; },
    5  // Initial size
);

auto conn = pool.acquire();
conn->query("SELECT * FROM users");
// Automatically returned to pool when shared_ptr is destroyed
```

## Suggested Next Steps

- Add object validation before reuse
- Implement object health checking
- Add pool statistics (hit rate, miss rate)
- Implement pool warming strategies
