# Union-Find (DSU)

Disjoint Set Union with path compression (iterative) and union by rank.

## Learning Goals

- find with full path compression: amortized near-constant time
- Why union by rank keeps trees flat
- Component-count bookkeeping: decrement only on real merges
- Iterative compression avoids deep-recursion stack risk on chains

## Non-Goals

- Weighted/ranked unions beyond rank heuristics
- Persistence / undo (rollback DSU)
- Concurrent access

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
