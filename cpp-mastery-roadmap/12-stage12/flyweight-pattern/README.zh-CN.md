# 享元模式

一个享元模式实现，用于共享公共数据。

## 学习目标

- 理解享元模式设计
- 练习共享状态管理
- 学习内部状态与外部状态
- 实现内存高效对象

## 非目标

- 构建生产级享元框架
- 实现复杂的共享策略
- 支持分布式享元

## 功能

- 带共享状态的 Font 享元
- 用于享元管理的 Font 工厂
- 带外部状态的 TextCharacter 上下文
- 内存高效的文本渲染

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/flyweight_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "flyweight_pattern.hpp"

flyweight_pattern::FontFactory factory;
auto font = factory.getFont("Arial", 12, false);

flyweight_pattern::TextCharacter ch1('A', font);
flyweight_pattern::TextCharacter ch2('B', font);
// ch1 和 ch2 共享同一个字体
```

## 建议的下一步

- 添加更多享元类型
- 实现享元池管理
- 添加享元统计
- 实现享元序列化
