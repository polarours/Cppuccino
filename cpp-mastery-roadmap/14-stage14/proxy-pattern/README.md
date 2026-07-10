# Proxy Pattern

A proxy pattern implementation for controlled access.

## Learning Goals

- Understand proxy pattern design
- Practice lazy loading
- Learn access control
- Implement virtual proxy

## Non-Goals

- Building a production proxy framework
- Implementing complex access control
- Supporting distributed proxies

## Features

- Image interface with display
- RealImage concrete subject
- ImageProxy lazy loading proxy
- On-demand object creation
- Access logging

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/proxy_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "proxy_pattern.hpp"

auto proxy = std::make_shared<proxy_pattern::ImageProxy>("photo.jpg");
proxy->display();  // Loads real image on first call
proxy->display();  // Uses cached image
```

## 建议的下一步

- 添加更多代理类型
- 实现代理缓存
- 添加代理访问控制
- 实现分布式代理
