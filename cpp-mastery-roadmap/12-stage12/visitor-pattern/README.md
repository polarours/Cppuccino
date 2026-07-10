# Visitor Pattern

A visitor pattern implementation for operations on object structures.

## Learning Goals

- Understand visitor pattern design
- Practice double dispatch
- Learn operation separation
- Implement extensible operations

## Non-Goals

- Building a production visitor framework
- Implementing complex visitor hierarchies
- Supporting distributed visitors

## Features

- Shape element interface with accept
- Visitor interface with visit methods
- Concrete shapes (Circle, Rectangle, Triangle)
- Concrete visitors (AreaCalculator, Printer)
- Double dispatch mechanism

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/visitor_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "visitor_pattern.hpp"

visitor_pattern::Circle circle(5.0);
visitor_pattern::AreaCalculator calculator;

circle.accept(calculator);
double area = calculator.getResult();
```

## 建议的下一步

- 添加更多访问者类型
- 实现访问者缓存
- 添加访问者验证
- 实现分布式访问者
