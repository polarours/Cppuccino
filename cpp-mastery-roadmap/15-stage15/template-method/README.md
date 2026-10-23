# Template Method

A template method pattern implementation for algorithm skeleton.

## Learning Goals

- Understand template method pattern
- Practice algorithm skeleton
- Learn hook methods
- Implement code reuse

## Non-Goals

- Building a production data mining framework
- Implementing complex parsing algorithms
- Supporting distributed mining

## Features

- DataMiner abstract class with template method
- CSV, JSON, XML concrete miners
- Hook methods for customization
- Consistent algorithm structure
- Code reuse through inheritance

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/template_method_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "template_method.hpp"

template_method::CSVDataMiner miner;
miner.mine("data.csv");
```

## Suggested Next Steps

- Add more miner types
- Implement hook method composition
- Add miner configuration
- Implement a miner pipeline
