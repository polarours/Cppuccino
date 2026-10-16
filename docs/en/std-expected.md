# std::expected — A Type-Safe Answer for Error Handling (C++23)

## What Is std::expected?

`std::expected<T, E>`, introduced in C++23, represents a value **or** an error:
it holds a `T` on success and an `E` on failure. The two mutually exclusive
states are encoded in the type system, so the compiler forces callers to
handle the error path.

> **Core idea**: the safe version of `std::optional` — not "maybe nothing", but
> "a value or an error".

## std::optional vs std::expected

| | std::optional | std::expected |
|---|---|---|
| Semantics | "maybe nothing" | "value or error" |
| Carries information on failure | No | Yes — the error type `E` |
| Monadic API | Yes | Yes (`and_then`/`transform`/`or_else`) |
| Fit | Optional parameters, returning empty when absent | Any function with a failure path |

## Core Concepts

- **Success path**: `return expected;` or `return value;` (implicit
  construction from `T`)
- **Failure path**: `return std::unexpected(error);`
- **Unwrapping**: test with `if (r)`, read the value with `*r`, the error with
  `.error()`, or fall back with `.value_or(default)`

## Implementation

```cpp
#include <expected>
#include <string>

enum class ErrorCode {
    None, NotFound, InvalidInput, NetworkError
};

std::string errorMessage(ErrorCode code) {
    switch (code) {
        case ErrorCode::NotFound:    return "Not found";
        case ErrorCode::InvalidInput: return "Invalid input";
        case ErrorCode::NetworkError: return "Network error";
        default: return "No error";
    }
}

// value-or-error return type
std::expected<int, ErrorCode> parseInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return std::unexpected(ErrorCode::InvalidInput);
    }
}

std::expected<double, ErrorCode> divide(double a, double b) {
    if (b == 0) return std::unexpected(ErrorCode::InvalidInput);
    return a / b;
}
```

For the full runnable example, see
[examples/std-expected.cpp](../../examples/std-expected.cpp).

## Monadic Operations (Chaining)

`std::expected` supports `and_then` / `transform` / `or_else`, letting errors
propagate naturally through a chain:

```cpp
#include <cassert>

int main() {
    // parseInt("10") → divide by 2 → 5.0
    auto result = parseInt("10")
        .and_then([](int v) { return divide(v, 2.0); });
    assert(result && *result == 5.0);

    // transform: changes the value type, keeps the error type
    auto doubled = parseInt("21")
        .transform([](int v) { return v * 2; });
    assert(doubled && *doubled == 42);

    // or_else: custom error handling
    auto safe = parseInt("abc")
        .or_else([](ErrorCode) -> std::expected<int, ErrorCode> {
            return std::unexpected(ErrorCode::NetworkError);
        });
    assert(!safe && safe.error() == ErrorCode::NetworkError);
    return 0;
}
```

## Best Practices

1. **Prefer `std::expected` over `std::pair<bool, T>`**: named interface, the
   type system enforces handling, and error details don't leak
2. **Prefer `std::unexpected` over `throw`**: the C++ convention is "functions
   that don't throw say so" — `expected` makes that contract explicit
3. **Use `enum class` for error types**: never confusable with the success
   type; reach for `std::error_code` or a custom struct when you need more
4. **Don't mix types inside a monadic chain**: the `and_then` closure must
   also return `std::expected`, or you get a compile error
5. **Be careful with `.value_or()`**: the default expression only runs when
   the expected holds an error — if computing the default is expensive,
   consider `or_else` instead

## How It Relates to std::error_code

Large systems should prefer `std::error_code` as the error type — it
composes, compares, and already lives in the standard-library ecosystem:

```cpp
#include <expected>
#include <system_error>
#include <string>

std::expected<int, std::error_code> readConfig(const std::string& key);
```

## Summary

`std::expected` is C++'s step from "errors via exceptions or return-value
conventions" toward "the type system enforces error handling". Together with
the monadic API it yields error-safe chained calls whose logic stays clear and
whose error branches cannot be forgotten.
