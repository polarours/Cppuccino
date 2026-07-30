# Reference Binding Demo

This project demonstrates C++ reference binding rules, a fundamental concept for understanding move semantics, forwarding references, and modern C++ API design.

## Learning Goals

- Understand lvalue reference binding rules
- Understand rvalue reference binding rules
- Learn how function parameters bind to references
- Understand reference collapsing and perfect forwarding
- Understand lifetime extension with const lvalue references

## Concepts Demonstrated

1. **Lvalue Reference Binding**: `int&` can only bind to lvalues
2. **Const Lvalue Reference Binding**: `const int&` can bind to both lvalues and rvalues (lifetime extension)
3. **Rvalue Reference Binding**: `int&&` only binds to rvalues (or explicitly moved lvalues)
4. **Function Parameter Binding**: How reference parameters interact with caller arguments
5. **Reference Collapsing**: Rules for `T&&` in templates (perfect forwarding)
6. **Lifetime Extension**: Temporary objects bound to `const&` live longer

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/reference_binding_demo
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Related Concepts

- Lvalue and Rvalue References (docs/en/lvalue-and-rvalue-references.md)
- Understanding the Semantics of `std::move` (docs/en/understanding-the-semantics-of-std::move.md)
- Forwarding References and Perfect Forwarding (docs/en/understanding-forwarding-references-and-perfect-forwarding.md)
- Move Semantics Example (examples/move-semantics-example.cpp)
