# 虚拟构造函数模式

一个虚拟构造函数模式实现，用于未知类型的克隆。

## 学习目标

- 理解虚拟构造函数模式
- 练习类型安全的克隆
- 学习工厂模式集成
- 实现多态克隆

## 非目标

- 构建生产级形状库
- 实现复杂的形状操作
- 支持分布式形状

## 功能

- 带 clone 的 Shape 接口
- 具体形状（Circle、Rectangle、Triangle）
- 用于创建的 ShapeFactory
- 类型安全的克隆
- 多态克隆

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/virtual_constructor_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

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
