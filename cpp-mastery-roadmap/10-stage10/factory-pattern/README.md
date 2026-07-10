# Factory Pattern

A factory pattern implementation for object creation.

## Learning Goals

- Understand factory pattern design
- Practice object creation abstraction
- Learn registry-based factories
- Implement auto-registration

## Non-Goals

- Building a production factory framework
- Implementing complex factory hierarchies
- Supporting distributed factories

## Features

- Product interface with name/price
- Concrete products (Book, Electronics, Clothing)
- Factory with type registry
- Auto-registration with static instances
- Runtime type creation

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/factory_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "factory_pattern.hpp"

auto book = factory_pattern::ProductFactory::create("book", "C++ Primer", 49.99);
auto electronics = factory_pattern::ProductFactory::create("electronics", "Laptop", 999.99);
```

## 建议的下一步

- 添加更多产品类型
- 实现工厂方法模式
- 添加产品原型模式
- 实现抽象工厂
