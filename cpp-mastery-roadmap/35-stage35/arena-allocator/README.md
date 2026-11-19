# Arena Allocator

Bump allocator for fast sequential allocation.

A request that cannot fit returns `nullptr`. The capacity check subtracts
instead of adding, so a huge `size` cannot wrap around and slip past it.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
