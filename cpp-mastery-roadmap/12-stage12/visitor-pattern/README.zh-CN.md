# 访问者模式

一个访问者模式实现，用于对象结构上的操作。

## 学习目标

- 理解访问者模式设计
- 练习双重分发
- 学习操作分离
- 实现可扩展操作

## 非目标

- 构建生产级访问者框架
- 实现复杂的访问者层次结构
- 支持分布式访问者

## 功能

- 带 accept 的 Shape 元素接口
- 带 visit 方法的 Visitor 接口
- 具体形状（Circle、Rectangle、Triangle）
- 具体访问者（AreaCalculator、Printer）
- 双重分发机制

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/visitor_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

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
