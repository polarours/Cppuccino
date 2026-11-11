# CQRS Pattern

Command Query Responsibility Segregation — separates reads from writes.

The numbers in a payload are numbers and nothing else. The functions of
the standard library stop at the first character they cannot read, so
without the check "1abc" would be read as 1 and the rest of the text
would be dropped without a word.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
