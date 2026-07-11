# 迭代器模式

一个迭代器模式实现，用于遍历集合。

## 学习目标

- 理解迭代器模式设计
- 练习集合遍历
- 学习抽象迭代
- 实现自定义迭代器

## 非目标

- 构建生产级迭代器框架
- 实现复杂的迭代算法
- 支持分布式迭代器

## 功能

- 带 hasNext/next 的 Iterator 接口
- 带 createIterator 的 Aggregate 接口
- 带项目的具体聚合
- 类型安全的迭代
- 自定义迭代器实现

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/iterator_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "iterator_pattern.hpp"

iterator_pattern::ConcreteAggregate<int> agg;
agg.add(1);
agg.add(2);
agg.add(3);

auto iter = agg.createIterator();
while (iter->hasNext()) {
    std::cout << iter->next() << "\n";
}
```

## 建议的下一步

- 添加更多聚合类型
- 实现反向迭代器
- 添加过滤迭代器
- 实现流式迭代器
