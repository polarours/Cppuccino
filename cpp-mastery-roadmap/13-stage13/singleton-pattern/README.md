# Singleton Pattern

A singleton pattern implementation for single instance management.

## Learning Goals

- Understand singleton pattern design
- Practice thread-safe initialization
- Learn lazy initialization
- Implement global access point

## Non-Goals

- Building a production singleton framework
- Implementing complex singleton variants
- Supporting distributed singletons

## Features

- Thread-safe singleton with mutex
- Lazy initialization
- Config singleton with key-value storage
- Global access point
- Thread safety

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/singleton_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "singleton_pattern.hpp"

auto& logger = singleton_pattern::Logger::getInstance();
logger.log("Hello");

auto& config = singleton_pattern::Config::getInstance();
config.set("host", "localhost");
```

## 建议的下一步

- 添加单例生命周期管理
- 实现单例依赖注入
- 添加单例测试支持
- 实现单例序列化
