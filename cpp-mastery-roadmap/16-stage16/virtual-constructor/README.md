# Virtual Constructor

A virtual constructor pattern for cloning without knowing type.

## Learning Goals

- Understand virtual constructor pattern
- Practice type-safe cloning
- Learn factory pattern integration
- Implement polymorphic cloning

## Non-Goals

- Building a production shape library
- Implementing complex shape operations
- Supporting distributed shapes

## Features

- Shape interface with clone
- Concrete shapes (Circle, Rectangle, Triangle)
- ShapeFactory for creation
- Type-safe cloning
- Polymorphic cloning

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/virtual_constructor_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "virtual_constructor.hpp"

auto circle = virtual_constructor::ShapeFactory::create(
    virtual_constructor::ShapeFactory::ShapeType::Circle, 5.0);
auto copy = circle->clone();
```

## 建议的下一步

- 添加更多形状类型
- 实现形状变换
- 添加形状序列化
- 实现形状组合
