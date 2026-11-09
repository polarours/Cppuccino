# Result Type

Type-safe error handling without exceptions.

`parseInt` refuses anything that is not a whole number. The number
functions of the standard library stop at the first character they
cannot read, so without the check "12abc" would be read as 12 and the
rest of the text would be dropped without a word.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
