# Iterator Invalidation

## 1. Problem Statement

In C++, iterators, references, and pointers into containers are only valid under specific structural conditions.
Container mutations can invalidate some or all of them.

Using an invalidated iterator/reference/pointer is undefined behavior.

This pitfall is common because APIs often look symmetric:

1. obtain iterator,
2. mutate container,
3. keep using iterator.

The sequence compiles, may appear to work, and then fails under different capacity/layout/runtime conditions.

---

## 2. Scope and Non-Goals

### 2.1 Scope

This article covers:

1. why invalidation happens,
2. typical invalidation patterns per container family,
3. safe mutation/iteration strategies,
4. review and migration checklists.

### 2.2 Non-Goals

We do not reproduce the full standard table verbatim.
Focus is practical engineering reasoning with high-frequency cases.

---

## 3. Minimal Example

```cpp
#include <vector>

std::vector<int> v{1, 2, 3};
auto it = v.begin();
v.push_back(4); // may reallocate
int x = *it;    // UB if reallocation occurred
```

If `push_back` triggers reallocation,
all old iterators/references/pointers into `v` become invalid.

---

## 4. Why Invalidation Happens

Containers manage internal storage and topology.
Mutating operations may:

1. move elements to new memory,
2. shift positions,
3. erase nodes,
4. change internal structure.

Any handle that assumed old storage/topology can become stale.

---

## 5. Handle Types: Different Names, Similar Risk

For many containers, invalidation affects not only iterators,
but also:

1. references to elements,
2. pointers to elements,
3. views/span-like adapters derived from old data.

Do not audit only iterators and ignore references/pointers.

---

## 6. Container Families and Typical Behavior

### 6.1 Contiguous Containers (`vector`, `string`)

Risk profile:

1. reallocation can invalidate all iterators/references/pointers,
2. insertion/erase can invalidate ranges at/after affected position,
3. `reserve`/capacity planning changes risk profile but does not remove it globally.

### 6.2 `deque`

Has segmented storage; invalidation rules are nuanced.
Some operations invalidate all iterators but preserve references in specific cases,
others invalidate local ranges.

Assume complexity unless operation-specific behavior is known.

### 6.3 Node-Based Containers (`list`, `forward_list`)

Insertion often keeps existing iterators stable.
Erasing a node invalidates iterators/references to erased node only.

Better iterator stability, but different performance trade-offs.

### 6.4 Associative/Unordered Containers

`map`/`set` (tree-based) generally provide strong iterator stability except erased elements.

`unordered_*` may invalidate iterators on rehash.
References/pointers to elements often have different guarantees than iterators.

---

## 7. Classic Failure Patterns

### 7.1 Mutate While Iterating with Cached End

```cpp
for (auto it = v.begin(), e = v.end(); it != e; ++it) {
	v.push_back(*it); // e may become invalid
}
```

Caching `end` in mutating loop is often wrong for invalidation-sensitive containers.

### 7.2 Erase in Loop with Incorrect Increment

```cpp
for (auto it = v.begin(); it != v.end(); ++it) {
	if (pred(*it)) {
		v.erase(it); // it invalidated; ++it afterwards is UB
	}
}
```

Correct pattern uses returned iterator from `erase`.

### 7.3 Store Pointer to `vector` Element Across Growth

```cpp
int* p = &v[0];
v.push_back(42);
use(*p); // may dangle after reallocation
```

---

## 8. Correct Erase-Loop Pattern

```cpp
for (auto it = v.begin(); it != v.end(); ) {
	if (pred(*it)) {
		it = v.erase(it); // returns next valid iterator
	} else {
		++it;
	}
}
```

This pattern is fundamental and should be standardized in team code style.

---

## 9. Capacity Planning Is Helpful, Not Absolute

`vector::reserve` can reduce reallocations:

```cpp
v.reserve(expected);
```

But it does not make all iterators universally stable.
Operations like erase/insert at positions still have local invalidation effects.

Treat reserve as optimization tool, not correctness proof.

---

## 10. Reacquire After Potentially Invalidating Operations

General safe rule:

1. after mutation that may invalidate handles,
2. reacquire iterators/references from container state.

Do not assume old handles remain valid by coincidence.

---

## 11. Index-Based Traversal Trade-Off

For contiguous containers,
index-based loops can sometimes avoid iterator invalidation pitfalls:

```cpp
for (std::size_t i = 0; i < v.size(); ++i) {
	// careful: size may change if mutating
}
```

Still requires clear mutation policy (`size` growth, erase shifts).
Not universally safer by default.

---

## 12. `erase-remove` Idiom

For removing elements by predicate in sequence containers:

```cpp
v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
```

This centralizes mutation and reduces manual iterator mistakes.

In C++20+, prefer `std::erase_if(v, pred)` where available.

---

## 13. Range-Based `for` and Mutation

Mutating container structure inside range-based loop over same container is often unsafe.

```cpp
for (auto& x : v) {
	if (x == 1) v.push_back(2); // dangerous for vector/string/deque patterns
}
```

Avoid unless operation semantics are explicitly known to preserve required handles.

---

## 14. Unordered Containers and Rehash Surprises

`unordered_map` / `unordered_set` can rehash when bucket count changes.
Rehash can invalidate iterators broadly.

Mitigations:

1. reserve bucket count in advance,
2. avoid long-lived iterators across insert-heavy phases,
3. reacquire iterators after operations that may trigger rehash.

---

## 15. Borrowed Views Are Also at Risk

If code builds `std::span`, `string_view`, or custom views from container storage,
container mutation can invalidate those views too.

Iterator-invalidation discipline should include all borrowed handles,
not just STL iterators.

---

## 16. Concurrency Amplifies the Problem

Concurrent mutation and iteration without proper synchronization can cause:

1. data races,
2. stale/dangling handles,
3. container invariants violations.

Thread safety and invalidation safety are related but distinct requirements.
You need both.

---

## 17. Performance vs Stability Trade-Off

Node-based containers offer stronger iterator stability,
but may have worse cache locality and higher allocation overhead.

Contiguous containers offer great locality,
but weaker handle stability during structural mutations.

Choose container based on access/mutation pattern,
not habit.

---

## 18. API Design Guidance

If API returns iterators/references into internal containers,
document invalidation conditions explicitly.

Better yet, where possible:

1. expose value snapshots,
2. expose stable IDs/handles managed by owner,
3. minimize external dependence on internal iterator stability.

---

## 19. Typical Anti-Patterns

1. Storing iterators as long-lived object members without mutation protocol.
2. Keeping pointers into vector/string across growth operations.
3. Mutating same container inside callback invoked during iteration.
4. Assuming all standard containers have similar invalidation semantics.
5. Treating successful tests as proof of validity.

---

## 20. Safer Mutation Patterns

1. Phase separation: collect changes first, apply mutations later.
2. Recompute handles after mutation.
3. Use return iterator from mutating operations (`erase`, `insert` where applicable).
4. Keep mutation scope narrow and local.

These patterns reduce cognitive load and bug surface.

---

## 21. Legacy Code Migration Strategy

1. Search for long-lived iterator/reference fields.
2. Audit loops that mutate during iteration.
3. Replace fragile loops with canonical erase/update patterns.
4. Introduce container-specific helper utilities.
5. Add regression tests for growth/rehash-heavy scenarios.

---

## 22. Tooling Support

Use sanitizers and debug iterators (where library/toolchain supports them):

1. ASan/UBSan for runtime UB symptoms,
2. debug iterator modes in STL implementations,
3. static analysis for suspicious mutation-after-capture patterns.

Tooling does not replace semantic understanding,
but catches many high-risk cases early.

---

## 23. Review Checklist

- [ ] Does code keep iterators/references/pointers across container mutation?
- [ ] Are container-specific invalidation rules known for each operation?
- [ ] Are erase loops using returned iterator pattern?
- [ ] Are views/spans/string_views derived from mutable container storage?
- [ ] Could reserve/rehash behavior invalidate assumptions?
- [ ] Is mutation and iteration protocol documented?

If any answer is uncertain, refactor for clearer safety.

---

## 24. Mitigation Summary

1. Learn invalidation rules per container and operation.
2. Reacquire handles after potentially invalidating mutations.
3. Prefer canonical loop patterns for erase/update.
4. Select container by required stability profile.
5. Make lifetime/invalidation assumptions explicit in API contracts.

---

## 25. Conclusion

Iterator invalidation is not an edge case;
it is a central correctness property of container usage.

Robust C++ code treats iterator/reference/pointer validity as a contract,
updated whenever container structure changes.

Once teams internalize this model,
an entire class of undefined behavior bugs becomes preventable through routine review discipline.
