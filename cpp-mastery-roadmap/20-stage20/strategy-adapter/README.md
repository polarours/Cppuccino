# Strategy Adapter

A strategy adapter for adapting incompatible interfaces.

## Learning Goals

- Understand adapter pattern in strategy context
- Practice interface adaptation
- Learn legacy code integration
- Implement interface bridging

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/strategy_adapter_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
