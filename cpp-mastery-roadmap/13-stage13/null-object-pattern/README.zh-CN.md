# 空对象模式

一个空对象模式实现，用于处理空值情况。

## 学习目标

- 理解空对象模式设计
- 练习安全的空值处理
- 学习默认行为实现
- 实现透明的空对象

## 非目标

- 构建生产级空对象框架
- 实现复杂的空对象行为
- 支持分布式空对象

## 功能

- 带 speak/move 的 Animal 接口
- 具体动物（Dog、Cat、Bird）
- 用于未知类型的 NullAnimal
- 用于动物创建的 Factory
- 安全的空值处理

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/null_object_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "null_object_pattern.hpp"

auto animal = null_object_pattern::AnimalFactory::create("unknown");
std::cout << animal->speak() << "\n";  // "(silence)"
```

## 建议的下一步

- 添加更多空对象类型
- 实现空对象日志
- 添加空对象统计
- 实现空对象序列化
