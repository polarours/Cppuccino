# Flyweight Pattern

A flyweight pattern implementation for sharing common data.

## Learning Goals

- Understand flyweight pattern design
- Practice shared state management
- Learn intrinsic vs extrinsic state
- Implement memory-efficient objects

## Non-Goals

- Building a production flyweight framework
- Implementing complex sharing strategies
- Supporting distributed flyweights

## Features

- Font flyweight with shared state
- Font factory for flyweight management
- Text character context with extrinsic state
- Memory-efficient text rendering

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/flyweight_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "flyweight_pattern.hpp"

flyweight_pattern::FontFactory factory;
auto font = factory.getFont("Arial", 12, false);

flyweight_pattern::TextCharacter ch1('A', font);
flyweight_pattern::TextCharacter ch2('B', font);
// ch1 and ch2 share the same font
```

## 建议的下一步

- 添加更多 flyweight 类型
- 实现 flyweight 池管理
- 添加 flyweight 统计
- 实现 flyweight 序列化
