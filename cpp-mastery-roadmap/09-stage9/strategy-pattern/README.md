# Strategy Pattern

A strategy pattern implementation for algorithm selection at runtime.

## Learning Goals

- Understand strategy pattern design
- Practice runtime algorithm selection
- Learn template-based strategy interfaces
- Implement strategy switching

## Non-Goals

- Building a production sorting library
- Implementing complex algorithm selection
- Supporting distributed algorithms

## Features

- Template-based strategy interface
- Multiple sorting strategies (Bubble, Quick, Insertion)
- Runtime strategy switching
- Strategy name query

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/strategy_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "strategy_pattern.hpp"

strategy_pattern::Sorter<int> sorter;
sorter.setStrategy(std::make_shared<strategy_pattern::QuickSort<int>>());

std::vector<int> data = {5, 3, 1, 4, 2};
sorter.sort(data);
// data is now sorted
```

## Suggested Next Steps

- Add more sorting strategies
- Implement strategy selection based on data characteristics
- Add strategy caching
- Implement distributed algorithms
