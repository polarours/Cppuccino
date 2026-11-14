# Expected Pattern

Type-safe value-or-error with monadic operations.

`divide()` refuses the one division whose result no `int` can hold: the
processor traps on it, so the program would die instead of reporting a
problem.

`map()` and `andThen()` work out the type of their result from a call
with an **lvalue**, because that is the call they actually make. A
function with an overload for lvalues and one for rvalues therefore
picks the same overload in both places.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
