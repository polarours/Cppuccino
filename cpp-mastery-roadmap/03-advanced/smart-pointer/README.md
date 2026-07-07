# Smart Pointer

Custom implementation of unique_ptr, shared_ptr, and weak_ptr.

## Learning Goals

- Understand reference counting mechanisms
- Practice RAII for memory management
- Learn how smart pointers work internally
- Implement move semantics and ownership transfer

## Non-Goals

- Building production-grade smart pointers
- Supporting custom deleters or allocators

## Features

- UniquePtr: exclusive ownership with move semantics
- SharedPtr: shared ownership with reference counting
- WeakPtr: non-owning reference to break cycles

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/smart_ptr_example
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Example

```cpp
#include "smart_ptr.hpp"

// Unique ownership
smart_ptr::UniquePtr<int> uptr(new int(42));
auto moved = std::move(uptr);  // uptr is now null

// Shared ownership
smart_ptr::SharedPtr<int> sptr(new int(100));
auto copy = sptr;  // Both share ownership
// use_count() == 2

// Weak reference
smart_ptr::WeakPtr<int> wptr = sptr;
if (auto locked = wptr.lock()) {
    // Use locked pointer
}
```

## Suggested Next Steps

- Add custom deleter support
- Implement make_unique and make_shared
- Add weak_ptr::lock() thread safety
- Add aliasing constructor
