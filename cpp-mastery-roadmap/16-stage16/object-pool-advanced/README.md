# Object Pool Advanced

An advanced object pool with validation and health checks.

## Learning Goals

- Understand advanced object pooling
- Practice object validation
- Learn health checking patterns
- Implement robust pooling

## Non-Goals

- Building a production pool framework
- Implementing complex validation rules
- Supporting distributed pooling

## Features

- Generic template-based pool
- Object validation before reuse
- Health checking support
- Thread-safe operations
- Configurable pool size

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/object_pool_advanced_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "object_pool_advanced.hpp"

object_pool_advanced::ObjectPool<Connection> pool(
    []() { return new Connection(); },
    [](Connection* p) { delete p; },
    [](const Connection* p) { return p->valid; },
    2, 10
);

auto conn = pool.acquire();
// Use connection
conn.reset();  // Returns to pool
```

## 建议的下一步

- 添加更多验证规则
- 实现连接健康检查
- 添加池统计
- 实现池监控
