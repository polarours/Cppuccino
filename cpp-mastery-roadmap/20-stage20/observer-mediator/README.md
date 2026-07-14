# Observer Mediator

An observer pattern combined with mediator for event coordination.

## Learning Goals

- Understand observer-mediator combination
- Practice event coordination
- Learn centralized event handling
- Implement event routing

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/observer_mediator_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
