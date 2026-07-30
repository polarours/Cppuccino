# Static Initialization Order Demo

This project demonstrates the **static initialization order fiasco** in C++ and shows a solution using function-scope static variables.

## Learning Goals

- Understand the static initialization order problem (also known as the "static initialization order fiasco")
- See why the order of initialization of global/static variables across translation units is unspecified
- Learn the solution using function-scope static variables (Meyers' singleton)

## Project Structure

- `src/bad_demo.cpp`: Global counter objects that demonstrate the initialization order issue
- `src/main.cpp`: Main program showing both the bad and good approaches
- `src/counter.cpp`: Counter class implementation with shared static state
- `include/counter.hpp`: Counter class header
- `tests/static_init_tests.cpp`: Unit tests

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/static_init_demo
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## The Problem

In C++, the order of initialization of non-local static variables across different translation units is unspecified. This can lead to undefined behavior if one global object uses another global object before it's been constructed.

## The Solution

Using function-scope static variables (Meyers' singleton) guarantees initialization on first use, avoiding the static initialization order problem entirely.
