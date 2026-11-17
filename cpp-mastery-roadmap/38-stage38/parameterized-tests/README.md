# Parameterized Tests

Data-driven test execution.

The numbers the tests are about do not overflow: the loop of `isPrime()`
asks the division instead of multiplying, and `fibonacci()` counts in a
`long long`, because the 47th Fibonacci number does not fit in an `int`.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
