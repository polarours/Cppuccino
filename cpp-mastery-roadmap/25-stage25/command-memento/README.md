# Command Memento

Command pattern with memento for state preservation.

`pop()` on an empty history hands back nothing, and `restore()` with
nothing to restore leaves the editor alone instead of bringing the
program down.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
