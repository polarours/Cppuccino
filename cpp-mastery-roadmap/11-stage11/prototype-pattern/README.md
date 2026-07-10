# Prototype Pattern

A prototype pattern implementation for object cloning.

## Learning Goals

- Understand prototype pattern design
- Practice object cloning
- Learn prototype registry
- Implement deep copy

## Non-Goals

- Building a production prototype framework
- Implementing complex cloning strategies
- Supporting distributed prototypes

## Features

- Shape prototype interface with clone()
- Concrete prototypes (Circle, Rectangle, Triangle)
- Shape registry for prototype management
- Deep copy semantics
- Independent clones

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/prototype_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "prototype_pattern.hpp"

auto circle = std::make_shared<prototype_pattern::Circle>(5.0);
auto cloned = circle->clone();  // Deep copy

prototype_pattern::ShapeRegistry registry;
registry.registerShape("circle", circle);
auto fromRegistry = registry.create("circle");
```

## 建议的下一步

- 添加更多原型类型
- 实现原型缓存
- 添加原型验证
- 实现原型序列化
