# Composite Pattern

A composite pattern implementation for tree structures.

## Learning Goals

- Understand composite pattern design
- Practice tree structure implementation
- Learn uniform interface for leaves and composites
- Implement recursive operations

## Non-Goals

- Building a production file system
- Implementing complex tree operations
- Supporting distributed composites

## Features

- FileSystemComponent interface
- File leaf class
- Directory composite class
- Nested structure support
- Size calculation

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/composite_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "composite_pattern.hpp"

auto root = std::make_shared<composite_pattern::Directory>("root");
root->add(std::make_shared<composite_pattern::File>("file.txt", 100));

std::cout << "Size: " << root->getSize() << "\n";
root->print();
```

## 建议的下一步

- 添加更多组件类型
- 实现树遍历算法
- 添加树操作（搜索、过滤）
- 实现树序列化
