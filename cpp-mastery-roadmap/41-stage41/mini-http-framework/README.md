# Mini HTTP Framework

A capstone project combining design patterns into a practical HTTP framework.

## Patterns Used

| Pattern | Where | Purpose |
|---------|-------|---------|
| **Middleware Pipeline** | `middleware.hpp` | Chained request processing |
| **Strategy** | `router.hpp` | Route matching |
| **Observer** | `event_bus.hpp` | Request/response events |
| **Command** | Handler registration | Reversible middleware |
| **Factory** | `Request::parse()` | Object creation |
| **Builder** | `Response::ok().header().body()` | Fluent response construction |
| **Type Erasure** | `handler.hpp` | Polymorphic handlers |
| **Facade** | `app.hpp` | Simplified interface |
| **Singleton** | `App::instance()` | Global app state |
| **Decorator** | Middleware wrapping | Add behavior |

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run Demo

```bash
./build/mini_http_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
