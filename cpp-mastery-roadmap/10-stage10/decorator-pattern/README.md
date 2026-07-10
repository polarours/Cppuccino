# Decorator Pattern

A decorator pattern implementation for adding behavior dynamically.

## Learning Goals

- Understand decorator pattern design
- Practice dynamic behavior composition
- Learn interface preservation
- Implement layered behavior

## Non-Goals

- Building a production decorator framework
- Implementing complex decorator chains
- Supporting distributed decorators

## Features

- Component interface with DataSource
- File data source concrete component
- Encryption, compression, logging decorators
- Composable decorators
- Transparent behavior addition

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/decorator_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "decorator_pattern.hpp"

auto source = std::make_shared<decorator_pattern::FileDataSource>("data.txt");
auto encrypted = std::make_shared<decorator_pattern::EncryptionDecorator>(source);
auto compressed = std::make_shared<decorator_pattern::CompressionDecorator>(encrypted);

// Read with encryption and compression
std::string data = compressed->readData();
```

## 建议的下一步

- 添加更多装饰器类型
- 实现装饰器链的性能优化
- 添加装饰器缓存
- 实现装饰器序列化
