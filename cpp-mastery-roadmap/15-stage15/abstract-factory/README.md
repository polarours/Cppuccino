# Abstract Factory

An abstract factory pattern implementation for creating families of objects.

## Learning Goals

- Understand abstract factory pattern
- Practice family object creation
- Learn platform-specific factories
- Implement consistent product families

## Non-Goals

- Building a production GUI framework
- Implementing complex widget hierarchies
- Supporting distributed factories

## Features

- Button and Checkbox abstract products
- Windows, Linux, Mac concrete products
- GUIFactory abstract factory
- Platform-specific concrete factories
- Consistent product families

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/abstract_factory_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "abstract_factory.hpp"

auto factory = std::make_unique<abstract_factory::LinuxFactory>();
auto button = factory->createButton();
auto checkbox = factory->createCheckbox();

button->render();    // [Linux Button]
checkbox->render();  // [Linux Checkbox]
```

## 建议的下一步

- 添加更多产品类型
- 实现工厂注册表
- 添加工厂缓存
- 实现工厂组合
