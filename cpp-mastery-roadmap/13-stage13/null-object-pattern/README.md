# Null Object Pattern

A null object pattern implementation for handling null cases.

## Learning Goals

- Understand null object pattern design
- Practice safe null handling
- Learn default behavior implementation
- Implement transparent null objects

## Non-Goals

- Building a production null object framework
- Implementing complex null behaviors
- Supporting distributed null objects

## Features

- Animal interface with speak/move
- Concrete animals (Dog, Cat, Bird)
- NullAnimal for unknown types
- Factory for animal creation
- Safe null handling

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/null_object_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "null_object_pattern.hpp"

auto animal = null_object_pattern::AnimalFactory::create("unknown");
std::cout << animal->speak() << "\n";  // "(silence)"
```

## 建议的下一步

- 添加更多 null 对象类型
- 实现 null 对象日志
- 添加 null 对象统计
- 实现 null 对象序列化
