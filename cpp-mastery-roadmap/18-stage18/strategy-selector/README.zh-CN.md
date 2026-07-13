# 策略选择器

一个动态策略选择系统。

## 学习目标

- 理解动态策略选择
- 练习基于注册表的查找
- 学习运行时算法切换
- 实现策略管理

## 非目标

- 构建生产级选择器
- 实现复杂的选择逻辑
- 支持分布式选择器

## 功能

- 带 sort/name 的策略接口
- BubbleSort 和 QuickSort 策略
- 带注册表的策略选择器
- 运行时策略选择
- 可用策略列表

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/strategy_selector_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "strategy_selector.hpp"

strategy_selector::StrategySelector selector;
selector.registerStrategy("bubble", std::make_shared<strategy_selector::BubbleStrategy>());

auto strategy = selector.select("bubble");
std::vector<int> data = {5, 3, 1, 4, 2};
strategy->sort(data);
```

## 建议的下一步

- 添加更多策略类型
- 实现策略评估
- 添加策略推荐
- 实现策略缓存
