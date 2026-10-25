# Move Semantics Trap: Moving from a `const` Object

## Problem Description

Trying to move from a `const` object is a common C++ trap. A move operation
expects to modify its source (typically draining it into an empty state), but a
`const` object is not allowed to be modified.

## Example: a Silent Copy, Not a Move

```cpp
#include <iostream>
#include <string>
#include <utility>

int main() {
    const std::string str = "hello";

    // The trap: this compiles, but it copies — it does not move
    std::string result = std::move(str);

    return result == "hello" ? 0 : 1;
}
```

**Not failing is the dangerous part**: overload resolution finds that the move
constructor `string(string&&)` cannot accept a `const string&&`, so it falls
back to the copy constructor `string(const string&)`. You think you transferred
the resource; every byte was actually copied.

Two situations that do fail to compile:

```cpp
#include <memory>
#include <string>
#include <utility>

void demo() {
    // Case A: a move-only type (copy deleted) → compile error
    const std::unique_ptr<int> p = std::make_unique<int>(42);
    auto q = std::move(p);

    // Case B: binding the result to a non-const reference → compile error
    const std::string str = "hello";
    std::string& ref = std::move(str);
}
```

## Why Does This Happen?

Move constructors and move assignment operators take a non-const rvalue
reference:

```cpp
class MyClass {
public:
    MyClass(MyClass&& other) noexcept;            // move constructor
    MyClass& operator=(MyClass&& other) noexcept; // move assignment
};
```

When you apply `std::move` to a `const` object:

1. `std::move` only performs a cast — it moves nothing by itself
2. The cast result is an rvalue reference of type `const T&&`
3. The move operation needs a *non-const* `T&&`; a `const T&&` cannot bind
4. Overload resolution falls back to the copy constructor `const T&`
   (for copyable types) → a **silent copy**; if the copy is deleted
   (move-only types) → a compile error

## Correct Approaches

### 1. Do not move from a const object

```cpp
#include <string>
#include <utility>

std::string str = "hello";
std::string result = std::move(str);  // correct: str is not const
```

### 2. Use a const reference for access

```cpp
#include <string>

const std::string str = "hello";
const std::string& ref = str;  // correct: read-only access
```

### 3. Copy explicitly when a copy is needed

```cpp
#include <string>

const std::string str = "hello";
std::string result = str;  // correct: a copy
```

### 4. Provide a const-safe "move" API

If you need to "move" in a `const` context (which is really a deep copy),
define an explicit interface for it:

```cpp
#include <utility>

class Data {
public:
    // move constructor
    Data(Data&& other) noexcept : data_(std::exchange(other.data_, nullptr)) {}

    // copy constructor (for const sources)
    Data(const Data& other) : data_(other.data_ ? new int(*other.data_) : nullptr) {}

    ~Data() { delete data_; }

private:
    int* data_;
};
```

## Related Traps

1. **Dangling references**: after a move, the source object's state is
   unspecified
2. **"Moving" from const**: copyable types silently copy, move-only types
   fail to compile
3. **Assuming `std::move` moves**: it is only a cast

## Example

A runnable version of this topic:

- [pitfall-const-object-move-trap.cpp](../../examples/pitfall-const-object-move-trap.cpp)

## Summary

- `std::move` is only a cast; it guarantees no actual move
- Move operations need a non-const rvalue reference
- Applying `std::move` to a `const` object does not move: copyable types
  silently copy, move-only types fail to compile
- If you must work from a `const` object, use copy semantics

> **Core principle: moving is "taking" — and `const` objects may not be taken
> from. If you need to transfer, make sure the source is not const.**
