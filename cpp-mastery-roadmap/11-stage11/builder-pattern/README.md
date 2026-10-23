# Builder Pattern

A builder pattern implementation for complex object construction.

## Learning Goals

- Understand builder pattern design
- Practice step-by-step construction
- Learn director and builder separation
- Implement fluent interface

## Non-Goals

- Building a production builder framework
- Implementing complex builder hierarchies
- Supporting distributed builders

## Features

- Computer product with components
- Builder interface with build steps
- Concrete builders (Gaming, Office, Workstation)
- Director for construction sequence
- Shared pointer for result management

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/builder_pattern_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "builder_pattern.hpp"

builder_pattern::ComputerDirector director;
director.setBuilder(std::make_shared<builder_pattern::GamingComputerBuilder>());
auto computer = director.construct();
```

## Suggested Next Steps

- Add more build steps
- Implement builder validation
- Add builder caching
- Implement builder serialization
