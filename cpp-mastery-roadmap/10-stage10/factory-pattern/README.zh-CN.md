# 工厂模式

一个工厂模式实现，用于对象创建。

## 学习目标

- 理解工厂模式设计
- 练习对象创建抽象
- 学习基于注册表的工厂
- 实现自动注册

## 非目标

- 构建生产级工厂框架
- 实现复杂的工厂层次结构
- 支持分布式工厂

## 功能

- 带 name/price 的产品接口
- 具体产品（书籍、电子产品、服装）
- 带类型注册表的工厂
- 带静态实例的自动注册
- 运行时类型创建

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/factory_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

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
