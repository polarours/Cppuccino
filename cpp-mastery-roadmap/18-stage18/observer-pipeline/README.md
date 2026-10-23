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

## Suggested Next Steps

- Add parallel processing stages
- Implement error handling
- Add a view of the intermediate results
- Implement pipeline caching
