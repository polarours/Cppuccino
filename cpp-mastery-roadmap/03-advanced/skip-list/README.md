# Skip List

Probabilistic ordered set: sorted singly-linked lists with randomized
levels, giving O(log n) expected search/insert.

## Learning Goals

- Tower of forward pointers and multi-level traversal
- randomLevel() with p=1/2 and a deterministic seed for tests
- Set semantics via three-way comparison (`!(a<b) && !(b<a)`)
- Why skip lists beat BSTs for concurrent/friendly implementations

## Non-Goals

- Deletion
- Thread safety
- Balanced worst-case guarantees (trees' territory)

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
