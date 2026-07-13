# 策略评估器

一个策略评估系统，用于性能比较。

## 学习目标

- 理解策略评估
- 练习性能测量
- 学习算法比较
- 实现基准测试

## 非目标

- 构建生产级基准测试系统
- 实现复杂的统计分析
- 支持分布式评估

## 功能

- 带 sort 的策略接口
- BubbleSort 和 QuickSort 策略
- 带计时的 EvaluationResult
- 用于比较的 StrategyEvaluator
- 最佳策略选择

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/strategy_evaluator_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "strategy_evaluator.hpp"

strategy_evaluator::StrategyEvaluator evaluator;
evaluator.evaluate(std::make_shared<strategy_evaluator::BubbleSort>(), data);
evaluator.evaluate(std::make_shared<strategy_evaluator::QuickSort>(), data);
evaluator.printResults();
```

## 建议的下一步

- 添加更多策略类型
- 实现统计分析
- 添加可视化
- 实现分布式评估
