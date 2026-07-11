# Iterator Pattern

An iterator pattern implementation for traversing collections.

## Learning Goals

- Understand iterator pattern design
- Practice collection traversal
- Learn abstract iteration
- Implement custom iterators

## Non-Goals

- Building a production iterator framework
- Implementing complex iteration algorithms
- Supporting distributed iterators

## Features

- Iterator interface with hasNext/next
- Aggregate interface with createIterator
- Concrete aggregate with items
- Type-safe iteration
- Custom iterator implementation

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/iterator_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

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
