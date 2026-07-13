# Strategy Selector

A dynamic strategy selection system.

## Learning Goals

- Understand dynamic strategy selection
- Practice registry-based lookup
- Learn runtime algorithm switching
- Implement strategy management

## Non-Goals

- Building a production selector
- Implementing complex selection logic
- Supporting distributed selectors

## Features

- Strategy interface with sort/name
- BubbleSort and QuickSort strategies
- Strategy selector with registry
- Runtime strategy selection
- Available strategies listing

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/strategy_selector_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

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
