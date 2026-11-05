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

## Thread Safety

The pool hands an object out and takes one back while it holds its lock,
so that two callers never get the same object. The factory, the deleter
and the validator therefore run under that lock: they must not call back
into the pool, because the mutex is not recursive and the call would
wait for itself.


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

#include <string>

// The pool hands out objects of the type you give it.
struct Connection {
    std::string query(const std::string& sql) {
        return "rows for " + sql;
    }
};

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
