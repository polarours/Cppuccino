# Connection Pool

A generic connection pool implementation for managing reusable resources.

## Learning Goals

- Understand resource pooling patterns
- Practice thread synchronization for concurrent access
- Learn connection lifecycle management
- Implement timeout and error handling

## Non-Goals

- Building a production connection pool
- Implementing connection health checking
- Supporting distributed connection pools

## Features

- Generic template-based pool
- Configurable min/max pool size
- Thread-safe acquire/release
- Connection reuse
- Timeout support
- Graceful shutdown

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/connection_pool_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "connection_pool.hpp"

connection_pool::ConnectionPool<Connection> pool(
    []() { return Connection(); },  // Creator
    5, 10  // Min/max size
);

auto conn = pool.acquire();
conn.query("SELECT * FROM users");
pool.release(conn);
```

## Suggested Next Steps

- Add connection health checking
- Implement connection validation
- Add connection statistics
- Implement connection retry logic
