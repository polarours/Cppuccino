# Binary Heap

Array-backed binary heap with custom ordering (max-heap by default),
O(log n) push/pop, and O(n) bottom-up heapify.

## Learning Goals

- Parent/child index math on a vector
- siftUp vs siftDown responsibilities
- Why heapify from n/2 down is O(n), not O(n log n)
- Comparator as a template parameter (std::less = max-heap)

## Non-Goals

- Decrease-key / arbitrary delete
- d-ary or Fibonacci heaps
- Thread safety

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
