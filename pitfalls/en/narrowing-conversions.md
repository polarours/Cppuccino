# Narrowing Conversions in Initialization

## The Problem

C++ brace initialization prevents narrowing conversions, but parenthesized
initialization silently truncates.

## Examples

```cpp
// This won't compile (brace init)
int x{3.14};  // ERROR: narrowing conversion

// This silently truncates (paren init)
int x(3.14);      // OK, x = 3 (truncated)
int y = (3.14);   // OK, y = 3 (truncated)

// Hidden truncation
long long big = 300;
char c = big;          // warning, but compiles
char c2{big};          // ERROR: narrowing
```

## Why It Matters

Narrowing conversions lose data silently. This is especially dangerous
with integer types of different sizes.

```cpp
uint8_t a = 256;       // wraps to 0 (no warning on some compilers)
uint8_t b = uint8_t(256);  // also wraps to 0

int16_t negative = -1;
uint16_t positive = negative;  // wraps to 65535
```

## Prevention

- Always use brace initialization: `int x{value};`
- Compile with `-Wconversion` and `-Wsign-conversion`
- Use explicit casts when truncation is intentional
- Prefer `static_cast` over implicit narrowing

## See Also

- [Implicit Conversions](implicit-conversions.md)
- [Signed Integer Overflow](signed-integer-overflow-assumptions.md)
