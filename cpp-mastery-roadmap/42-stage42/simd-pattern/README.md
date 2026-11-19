# SIMD Pattern

Manual SIMD-style vectorization for math operations.

The element-wise operations read both vectors at the same index, so a
shorter operand limits the result instead of reading past its end.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
