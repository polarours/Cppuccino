# Generator Pipeline (C++23)

Lazy pull-based pipelines over `std::generator`: iota / filter / transform /
take / fold, with early exit propagating all the way upstream.

## Learning Goals

- Writing a stage as a coroutine: the whole stage is a generator
- Type evolution through stages (`int -> std::string` via transform)
- Proving laziness: run-count assertions before/after partial consumption
- Contrast with ranges views: same semantics, hand-built machinery

## Non-Goals

- Parallel execution / two-way channels
- Error propagation inside pipelines
- ranges adaptor sugar (see examples/cpp23-ranges-views.cpp)

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
