# Observer Filter Chain

Observer with chained event filters.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
