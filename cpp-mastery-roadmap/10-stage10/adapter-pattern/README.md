# Adapter Pattern

An adapter pattern implementation for interface compatibility.

## Learning Goals

- Understand adapter pattern design
- Practice interface compatibility
- Learn class and object adapters
- Implement legacy system integration

## Non-Goals

- Building a production adapter framework
- Implementing complex adapter hierarchies
- Supporting distributed adapters

## Features

- MediaPlayer target interface
- VlcPlayer and Mp3Player adaptees
- VlcAdapter and Mp3Adapter adapters
- Common interface for all players
- Transparent adapter behavior

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/adapter_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "adapter_pattern.hpp"

auto vlc = std::make_shared<adapter_pattern::VlcPlayer>();
auto adapter = std::make_shared<adapter_pattern::VlcAdapter>(vlc);

// Use common MediaPlayer interface
adapter->play("test.vlc");
adapter->stop();
```

## 建议的下一步

- 添加更多适配器类型
- 实现双向适配器
- 添加适配器缓存
- 实现适配器链
