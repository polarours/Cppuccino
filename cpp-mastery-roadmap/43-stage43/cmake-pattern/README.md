# CMake Pattern

Programmatic CMakeLists.txt generation.

The generated file is meant to work: the tests link against the libraries
of the project, the defines are written out with `add_compile_definitions`,
and the whole thing configures, builds and passes its tests.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
