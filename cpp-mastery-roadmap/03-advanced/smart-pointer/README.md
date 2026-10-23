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

## Control block

The shared and the weak pointer share one control block, and the block
holds two counts:

- `strong_count` counts the `SharedPtr`s.
- `weak_count` counts the `WeakPtr`s **plus one**. That extra reference is
  held for as long as the strong count is non-zero.

The block dies with its last weak reference, never with its last strong
one. A `WeakPtr` has to stay able to ask whether the object is still
alive, so the block may not disappear while a `WeakPtr` is around.

The extra weak reference is what makes the other direction safe as well.
When the last `SharedPtr` goes away the object is destroyed, and its
destructor may release weak references of its own -- a node that points
back at itself, for instance. The block has to outlive that destructor, so
the extra weak reference is dropped only after it has run.

## Suggested Next Steps

- Add custom deleter support
- Implement make_unique and make_shared
- Add weak_ptr::lock() thread safety
- Add aliasing constructor
