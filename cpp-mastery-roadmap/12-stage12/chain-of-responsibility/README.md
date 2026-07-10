# Chain of Responsibility

A chain of responsibility pattern implementation for request handling.

## Learning Goals

- Understand chain of responsibility pattern
- Practice request handling chains
- Learn handler chaining
- Implement flexible request processing

## Non-Goals

- Building a production handler framework
- Implementing complex handler hierarchies
- Supporting distributed handlers

## Features

- Request struct with content and priority
- Handler interface with chaining
- Concrete handlers (Auth, Logging, Validation, Processing)
- Chain composition
- Request filtering

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/chain_of_responsibility_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "chain_of_responsibility.hpp"

auto auth = std::make_shared<chain_of_responsibility::AuthHandler>();
auto logging = std::make_shared<chain_of_responsibility::LoggingHandler>();
auto processing = std::make_shared<chain_of_responsibility::ProcessingHandler>();

auth->setNext(logging);
logging->setNext(processing);

chain_of_responsibility::Request req{"test", 1};
auth->handle(req);
```

## 建议的下一步

- 添加更多处理器类型
- 实现处理器优先级
- 添加处理器缓存
- 实现分布式处理器
