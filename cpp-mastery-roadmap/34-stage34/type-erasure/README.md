# Type Erasure

Hand-rolled std::function-style wrapper (AnyAction) and a type-erased
printable value (AnyPrintable): Concept interface + templated Holder/Model.

An action that was moved from holds nothing, and calling it throws
`std::bad_function_call` -- the same as an empty `std::function`, which
this class is modelled on. A value that was moved from says so when it
is asked for its text.

## Learning Goals

- The Concept/Holder split: virtual dispatch outside, template inside
- Why cloning the holder gives value-like copy semantics
- Rule of five for a polymorphic wrapper
- Compare with virtual-base approaches: smaller interface, harder to debug

## Non-Goals

- Small buffer optimization (SBO)
- RTTI-free variants
- Full std::function exception guarantees

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
