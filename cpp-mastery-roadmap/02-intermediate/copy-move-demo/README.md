# Copy & Move Semantics Demo

A demo project that prints every copy/move operation, so you can *see* which constructor/assignment operator the compiler picks in each situation.

## Learning Goals

- See the difference between copy and move construction/assignment
- Understand when the compiler calls each of the "big five" operations
- Observe lifetime semantics: what happens after a move, what `const&` extends

## Demonstrated Patterns

- **Copy object**: copy construction from an lvalue, copy assignment
- **Move object**: move construction via `std::move`, move assignment
- **Pass by value**: a temporary is constructed, then moved (or copy-elided)
- **Pass by reference**: lvalue binding and lifetime extension for `const&`

Every operation prints `[CONSTRUCT] / [COPY CTOR] / [MOVE CTOR] / [COPY ASSGN] / [MOVE ASSGN] / [DESTRUCT]` with object labels, so the trace is self-explanatory.

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/copy_move_demo
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Related Docs

- [Lvalue and Rvalue References](../../../docs/en/lvalue-and-rvalue-references.md)
- [Move Semantics Example](../../../examples/move-semantics-example.cpp)
