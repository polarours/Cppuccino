# Dangling `std::string_view`

## 1. Problem Statement

`std::string_view` is a lightweight non-owning view over character sequence data.
Its core benefit is avoiding allocation and copying.
Its core risk is lifetime mismatch.

`string_view` does **not** own storage and does **not** extend storage lifetime.
If underlying memory is destroyed, reallocated, or mutated unexpectedly,
the view becomes dangling or semantically invalid.

This pitfall is common because APIs using `string_view` often look safe and efficient,
while hidden lifetime assumptions remain undocumented.

---

## 2. Scope and Non-Goals

### 2.1 Scope

This article covers:

1. why dangling `string_view` happens,
2. high-frequency bug patterns,
3. robust replacement and API contract strategies,
4. review and migration checklists.

### 2.2 Non-Goals

This article does not discourage `string_view` usage in general.
When lifetime contracts are explicit, `string_view` is an excellent tool.

---

## 3. Minimal Bug Example

```cpp
#include <string>
#include <string_view>

std::string_view bad() {
    return std::string("temporary"); // view to destroyed temporary
}
```

The returned view points to dead memory.
Any access is undefined behavior.

---

## 4. Why It Fails

`std::string("temporary")` creates a temporary string object.
Its lifetime ends at end of full expression in `return` statement context.

`string_view` stores pointer + length,
but no ownership token.

After temporary destruction:

1. pointer is stale,
2. length is stale metadata,
3. reads are UB.

---

## 5. `string_view` Is a Borrowing Type

Treat `string_view` as borrow, not value-owner.

Borrow rule:

> producer lifetime must dominate consumer usage interval.

If this rule cannot be guaranteed, use owning type (`std::string`).

---

## 6. Frequent Dangling Patterns

### 6.1 Returning View to Local String

```cpp
std::string_view f() {
    std::string s = "abc";
    return s; // dangling
}
```

### 6.2 Returning View to Temporary Result

```cpp
std::string_view g() {
    return std::string("x") + "y"; // temporary destroyed
}
```

### 6.3 Storing View to Transient Input

```cpp
struct Holder {
    std::string_view v;
};

Holder make_holder() {
    std::string tmp = "abc";
    return Holder{tmp}; // Holder::v dangles
}
```

### 6.4 Capturing View in Async Callback

Callback runs later after original storage dies.

### 6.5 View into Container String That Reallocates

If underlying `std::string` mutates and reallocates, prior view may become invalid.

---

## 7. “Works in Tests” Is Not Evidence

Dangling views may appear valid in small tests because memory is not immediately overwritten.
Under optimization, different allocator behavior, or timing shifts,
the same code can fail unpredictably.

Correctness requires lifetime proof, not empirical luck.

---

## 8. Distinguish Two Classes of Invalidity

### 8.1 Lifetime Invalidity

Backing storage no longer exists -> UB.

### 8.2 Content/Range Invalidity

Storage exists but content changed unexpectedly, length assumptions stale,
or encoding invariants no longer hold.

Both are dangerous; only first is immediate UB.

---

## 9. Safe Return-Type Strategy

If function must return data beyond callee scope,
return owning type:

```cpp
std::string make_name() {
    return std::string("name");
}
```

Use `string_view` for read-only parameters or views into externally-owned stable storage.

---

## 10. Good Parameter Use Cases

Parameter `std::string_view` is often ideal when:

1. callee does not store view,
2. callee uses data only during call,
3. API accepts both `std::string` and string literals efficiently.

Example:

```cpp
void log_line(std::string_view msg);
```

This avoids overload explosion while keeping caller ownership unchanged.

---

## 11. Dangerous Storage Patterns

Avoid storing `string_view` in long-lived objects unless owner lifetime is explicit and enforced.

High-risk examples:

1. config registry storing views into temporary parse buffers,
2. cache keys as views to request-local strings,
3. event payload views crossing thread boundaries.

---

## 12. Class Design Rule

If class stores text beyond call boundary,
prefer `std::string` member by default.

Use `string_view` member only when class is explicitly a non-owning view type
and constructor/documentation define lifetime preconditions clearly.

---

## 13. API Contract Documentation Template

For any API taking or returning `string_view`, document:

1. ownership model (borrowed vs owned),
2. required backing-storage lifetime,
3. whether view is stored internally,
4. thread/async usage constraints.

Without this contract, misuse is likely.

---

## 14. `substr` and View Chaining

`std::string_view::substr` produces another view.
It does not allocate and does not extend lifetime.

Chaining views is cheap but lifetime-neutral.
All derived views depend on original storage still being valid.

---

## 15. Interaction with `std::string` Mutations

Views into `std::string` may be invalidated if string is modified in ways that reallocate.

This includes operations like append/assign/reserve in many cases.

Rule:

1. either freeze owner while view is in use,
2. or reacquire view after mutation.

---

## 16. Threading and `string_view`

`string_view` itself is tiny and trivially copyable,
but copying the view is not copying underlying data.

If another thread destroys or mutates owner concurrently,
view consumers can observe races or UB.

Treat owner synchronization as primary concern.

---

## 17. Asynchronous Systems Risk Amplifier

In async pipelines,
callbacks may execute far from capture/creation point.
Borrowed views crossing queue boundaries are frequent failure sources.

Safe default for async boundaries:

1. convert to owning `std::string`,
2. then pass forward.

---

## 18. Safer Conversion Boundary Pattern

```cpp
void enqueue(std::string s);

void enqueue_view(std::string_view sv) {
    enqueue(std::string(sv)); // ownership boundary explicit
}
```

This introduces copy cost but removes lifetime ambiguity.

---

## 19. Performance Perspective

`string_view` can reduce copies in hot paths,
but dangling bugs are expensive in production.

Optimization sequence:

1. establish lifetime-safe baseline,
2. profile,
3. optimize selected paths with clear contracts.

Never trade correctness for speculative micro-optimization.

---

## 20. Static Analysis and Tooling

Useful checks:

1. compiler warnings for returning references/views to temporaries (where available),
2. static analyzers that model escape/lifetime,
3. AddressSanitizer for many dangling access manifestations.

Tooling helps, but cannot infer every lifetime contract.

---

## 21. Common Anti-Patterns

1. Returning `string_view` from functions that build temporary strings.
2. Storing parameter `string_view` directly in persistent member.
3. Capturing `string_view` by value in async callback while owner is transient.
4. Using `string_view` as map key where referenced storage is mutable/transient.
5. Assuming literals and temporaries have equivalent lifetime.

---

## 22. Migration Guidance for Legacy Code

1. Search for `string_view` return types.
2. Audit member fields of type `string_view`.
3. Mark APIs as borrowing vs owning explicitly.
4. Convert risky storage sites to `std::string`.
5. Add tests around async and mutation-heavy paths.

This migration often prevents hard-to-reproduce production incidents.

---

## 23. Review Checklist

- [ ] Does any `string_view` outlive referenced storage?
- [ ] Are returned views guaranteed to reference stable storage?
- [ ] Are stored views documented as borrowed with explicit lifetime assumptions?
- [ ] Do async paths convert borrowed views to owning strings at boundary?
- [ ] Do mutating owner operations invalidate active views?
- [ ] Are API contracts explicit about lifetime preconditions?

If any item is uncertain, prefer ownership.

---

## 24. Correct Strategy Summary

1. Return `std::string` when ownership must escape.
2. Use `string_view` for read-only, non-storing call-path parameters.
3. Treat `string_view` members as advanced borrowing design requiring strict contracts.
4. Convert to owning string at async or storage boundaries.
5. Document lifetime assumptions in every public API.

---

## 25. Conclusion

`std::string_view` is a high-value performance and API ergonomics tool,
but only under explicit lifetime discipline.

Its non-owning nature is both strength and hazard.

Engineering rule of thumb:

> when lifetime is obvious and bounded, `string_view` is excellent;
> when lifetime is uncertain or crossing boundaries, own the string.

Correctness first, then optimization.
