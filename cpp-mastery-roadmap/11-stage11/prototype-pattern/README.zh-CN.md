# 原型模式

一个原型模式实现，用于对象克隆。

## 学习目标

- 理解原型模式设计
- 练习对象克隆
- 学习原型注册表
- 实现深拷贝

## 非目标

- 构建生产级原型框架
- 实现复杂的克隆策略
- 支持分布式原型

## 功能

- 带 clone() 的 Shape 原型接口
- 具体原型（Circle、Rectangle、Triangle）
- 用于原型管理的 Shape 注册表
- 深拷贝语义
- 独立的克隆对象

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/prototype_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "prototype_pattern.hpp"

auto circle = std::make_shared<prototype_pattern::Circle>(5.0);
auto cloned = circle->clone();  // 深拷贝

prototype_pattern::ShapeRegistry registry;
registry.registerShape("circle", circle);
auto fromRegistry = registry.create("circle");
```

## 建议的下一步

- 添加更多原型类型
- 实现原型缓存
- 添加原型验证
- 实现原型序列化
