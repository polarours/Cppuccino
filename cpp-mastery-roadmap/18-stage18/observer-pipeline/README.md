# Observer Pipeline

A pipeline for chained data processing.

## Learning Goals

- Understand pipeline pattern
- Practice data transformation chains
- Learn stage composition
- Implement sequential processing

## Non-Goals

- Building a production pipeline
- Implementing complex transformations
- Supporting distributed pipelines

## Features

- Pipeline template with stages
- Stage composition
- String processing helpers
- Sequential processing
- Type-safe pipelines

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/observer_pipeline_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "observer_pipeline.hpp"

observer_pipeline::Pipeline<std::string> pipeline;
pipeline.addStage(observer_pipeline::trim);
pipeline.addStage(observer_pipeline::toUpper);

std::string result = pipeline.process("  hello  ");
// result == "HELLO"
```

## 建议的下一步

- 添加并行处理阶段
- 实现错误处理
- 添加中间结果查看
- 实现管道缓存
