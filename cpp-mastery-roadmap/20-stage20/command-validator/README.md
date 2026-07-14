# Command Validator

A command validation system.

## Learning Goals

- Understand command validation
- Practice validation rules
- Learn error reporting
- Implement validation chains

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/command_validator_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
