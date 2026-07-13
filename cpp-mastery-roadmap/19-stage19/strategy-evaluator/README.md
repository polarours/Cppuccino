# Strategy Evaluator

A strategy evaluation system for performance comparison.

## Learning Goals

- Understand strategy evaluation
- Practice performance measurement
- Learn algorithm comparison
- Implement benchmarking

## Non-Goals

- Building a production benchmarking system
- Implementing complex statistics
- Supporting distributed evaluation

## Features

- Strategy interface with sort
- BubbleSort and QuickSort strategies
- EvaluationResult with timing
- StrategyEvaluator for comparison
- Best strategy selection

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/strategy_evaluator_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

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
