# 策略模式

一个策略模式实现，用于运行时算法选择。

## 学习目标

- 理解策略模式设计
- 练习运行时算法选择
- 学习基于模板的策略接口
- 实现策略切换

## 非目标

- 构建生产级排序库
- 实现复杂的算法选择
- 支持分布式算法

## 功能

- 基于模板的策略接口
- 多种排序策略（冒泡、快速、插入）
- 运行时策略切换
- 策略名称查询

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/strategy_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "strategy_pattern.hpp"

strategy_pattern::Sorter<int> sorter;
sorter.setStrategy(std::make_shared<strategy_pattern::QuickSort<int>>());

std::vector<int> data = {5, 3, 1, 4, 2};
sorter.sort(data);
// data 现在已排序
```

## 建议的下一步

- 添加更多排序策略
- 实现基于数据特征的策略选择
- 添加策略缓存
- 实现分布式算法
