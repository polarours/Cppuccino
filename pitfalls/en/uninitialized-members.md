# Uninitialized Member Variables

## The Problem

In C++, member variables are only zero-initialized for certain types.
For POD types (int, float, pointers), leaving them uninitialized means
they contain garbage values.

## Examples

```cpp
class Bad {
    int x;         // uninitialized!
    float y;       // uninitialized!
    int* ptr;      // uninitialized!
};

Bad b;
// b.x, b.y, b.ptr all have garbage values
// Using them is undefined behavior
```

```cpp
class Good {
    int x = 0;         // zero-initialized
    float y = 0.0f;    // zero-initialized
    int* ptr = nullptr; // null-initialized
    std::string name_;  // default-constructed (empty string)
};
```

## Why It Matters

Uninitialized variables are a major source of bugs that manifest
differently on different compilers, platforms, or even optimization levels.
A bug that works in debug mode may crash in release mode.

## Prevention

- Always initialize member variables at declaration
- Use in-class member initializers (NSDMI): `int x = 0;`
- Enable compiler warnings: `-Wall -Wuninitialized`
- Use `= default` for constructors when members have initializers
- Prefer `struct` with default initializers for simple data types

## See Also

- [Object Lifetime](object-lifetime-and-destruction-order-pitfalls.md)
- [Signed Integer Overflow](signed-integer-overflow-assumptions.md)
