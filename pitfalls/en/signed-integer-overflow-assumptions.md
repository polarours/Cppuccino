# Signed Integer Overflow Assumptions

## 0. Scope and Boundary

This chapter addresses a recurring C++ bug pattern:

- code assumes signed integer arithmetic wraps around like two's complement hardware,
- optimization then exploits undefined behavior (UB) rules,
- resulting program behavior diverges from developer intuition.

Boundary of this article:

1. We focus on **signed integer overflow** in standard C++ semantics.
2. We do **not** assume any implementation-specific wrapping mode unless explicitly enabled.
3. We discuss language rules first, then engineering practices.

Primary conclusion:

- In standard C++, signed overflow is UB.
- Once UB is reachable, optimizer transformations that seem “surprising” are still legal.
- Safety requires proving no overflow or using explicit checked/modular strategies.

---

## 1. Rule First: What the Language Allows

For signed integer types (`int`, `long`, `long long`, etc.), arithmetic overflow is not defined to wrap.

That means expressions such as:

- `x + y` (if result not representable),
- `x - y` (if result not representable),
- `x * y` (if result not representable),
- `-x` for the minimum value (e.g., `-INT_MIN`),

all trigger UB when outside representable range.

Important distinction:

- **Unsigned** overflow is defined modulo $2^N$.
- **Signed** overflow is UB.

Do not transfer the unsigned rule to signed code.

---

## 2. Why Optimizers Become Aggressive

Compilers optimize under the assumption that well-defined programs do not execute UB.

Therefore, they may reason:

1. “If this path implies signed overflow, that path is impossible in a well-defined execution.”
2. “Given impossible path elimination, condition simplifies.”
3. “Simplified condition enables further inlining and dead-code elimination.”

This produces control-flow transformations that look like logic bugs but are standard-conforming.

Mathematically, if overflow is excluded, monotonic properties hold and can be folded.

---

## 3. Canonical Misleading Example

```cpp
int f(int x) {
    return x + 1 > x; // may optimize to always true
}
```

Reasoning under “no UB” assumption:

- If `x == INT_MAX`, `x + 1` would overflow.
- UB is assumed impossible in defined executions.
- So the optimizer may treat `x == INT_MAX` as impossible here.
- Then `x + 1 > x` becomes always true.

This is legal for UB-free assumptions.

---

## 4. Real-World Bug Shapes

## 4.1 Range checks written after arithmetic

```cpp
int add_bad(int a, int b) {
    int c = a + b; // overflow may already happen
    if (c < a) {
        // intended overflow handling, but too late for signed UB
    }
    return c;
}
```

Problem: UB can occur before detection.

### 4.2 Loop counters near limits

```cpp
for (int i = start; i <= end; ++i) {
    // if end == INT_MAX, increment at boundary may overflow
}
```

### 4.3 Multiplication for allocation sizes

```cpp
int n = rows * cols; // potential overflow
std::vector<int> v(static_cast<std::size_t>(n));
```

Overflow may corrupt size computations and later memory logic.

### 4.4 Negating minimum value

```cpp
int abs_bad(int x) {
    return x < 0 ? -x : x; // UB when x == INT_MIN
}
```

### 4.5 Sentinel arithmetic

```cpp
int timeout = base + delta; // base may be near INT_MAX
```

Code silently depends on wraparound that signed rules do not provide.

---

## 5. Unsigned Is Not a Universal Fix

A common reaction is “switch everything to unsigned.”

This is not generally correct:

1. Unsigned wrap is defined, but semantics may be wrong for domain logic.
2. Comparisons between signed and unsigned can produce surprising conversions.
3. Negative sentinel values become impossible or confusing.

Use unsigned only when modular arithmetic is truly intended and documented.

Correctness criterion is semantic fit, not merely UB avoidance.

---

## 6. Correct Pattern A: Precondition Checks Before Arithmetic

Safe addition (int):

```cpp
#include <limits>

bool add_checked(int a, int b, int& out) {
    if (b > 0 && a > std::numeric_limits<int>::max() - b) {
        return false;
    }
    if (b < 0 && a < std::numeric_limits<int>::min() - b) {
        return false;
    }
    out = a + b;
    return true;
}
```

Key property:

- arithmetic is performed only when representability is proven.

Same principle applies to subtraction and multiplication.

---

## 7. Correct Pattern B: Wider Type with Verified Bound

Sometimes compute in a wider type and validate before narrowing:

```cpp
#include <cstdint>
#include <limits>

bool sum_to_int(int a, int b, int& out) {
    std::int64_t t = static_cast<std::int64_t>(a) + static_cast<std::int64_t>(b);
    if (t < std::numeric_limits<int>::min() || t > std::numeric_limits<int>::max()) {
        return false;
    }
    out = static_cast<int>(t);
    return true;
}
```

This approach is practical but still requires explicit range checks.

---

## 8. Correct Pattern C: Explicit Modular Semantics

If business logic genuinely requires modulo arithmetic:

1. Use fixed-width unsigned types (`std::uint32_t`, `std::uint64_t`).
2. Document that wraparound is part of contract.
3. Avoid mixing with signed types in boundary logic.

Example:

```cpp
#include <cstdint>

std::uint32_t step_mod32(std::uint32_t x) {
    return x + 1u; // defined modulo 2^32
}
```

Do not call this “overflow-safe signed arithmetic”; it is a different semantic model.

---

## 9. Correct Pattern D: Checked Arithmetic Utilities

In larger systems, centralize overflow checks in reusable utilities:

- `checked_add`, `checked_sub`, `checked_mul`,
- return `bool`, `std::optional<T>`, or error code,
- keep call sites concise and auditable.

Example style:

```cpp
template <typename T>
struct Checked {
    T value{};
    bool ok{false};
};
```

Then compose domain logic without re-implementing edge checks repeatedly.

---

## 10. Security and Reliability Impact

Signed overflow bugs are not merely “numeric inaccuracies.”

They can become:

1. memory allocation size errors,
2. bounds check bypasses,
3. integer truncation chains,
4. exploitable conditions in parsing/serialization paths.

In threat models involving untrusted input, arithmetic correctness is part of security posture.

---

## 11. Testing Strategy

### 11.1 Boundary-driven unit tests

Cover cases around:

- `min`, `min + 1`, `-1`, `0`, `1`, `max - 1`, `max`.

### 11.2 Property tests

For checked operations:

- if `ok == true`, recompute in wider type and assert equality,
- if `ok == false`, assert mathematical result is out of representable range.

### 11.3 Differential tests

Compare against a big-integer reference model for selected ranges.

---

## 12. Tooling in CI

Recommended layered defense:

1. High warning levels and warning-as-error policy.
2. Undefined behavior sanitizer and integer-focused sanitizer coverage in test pipelines.
3. Static analysis rules for suspicious arithmetic and narrowing conversions.
4. Fuzz tests for parsers/protocol handlers with arithmetic-heavy code paths.

No single tool is sufficient; combine static + dynamic + review process.

---

## 13. Code Review Checklist

When reviewing arithmetic code, ask:

1. Can operands originate from untrusted or unbounded input?
2. Is representable range proven before operation?
3. Is there any signed/unsigned implicit conversion?
4. Does logic accidentally rely on signed wraparound?
5. Are narrowing casts validated?
6. Is behavior consistent across 32-bit and 64-bit targets?

Red flags:

- post-check after potentially overflowing expression,
- `abs(int)` style negation without `INT_MIN` handling,
- multiplication in size/path/count calculations without guard.

---

## 14. Migration Plan for Legacy Code

For old codebases with implicit assumptions:

1. Inventory arithmetic hotspots (allocation, indexing, loop bounds, protocol lengths).
2. Add instrumentation builds with sanitizers.
3. Replace ad-hoc math with checked helpers.
4. Refactor APIs to use explicit range-aware types where possible.
5. Add boundary regression tests and keep them mandatory in CI.

Prefer incremental risk reduction instead of broad, unverified rewrites.

---

## 15. “Performance vs Safety” Framing

A common objection is branch cost in checked arithmetic.

Practical rule:

$$
	ext{Proven correctness} \gg \text{assumed speedup from UB-prone math}
$$

Then optimize where profiling shows material impact.

In many real systems, correctness and observability dominate arithmetic micro-costs.

---

## 16. Compact Bad/Good Matrix

| Intent | Fragile Pattern | Robust Pattern |
|---|---|---|
| add with overflow handling | compute then check | check bounds then compute |
| preserve mathematical sum | signed narrow type only | wider intermediate + explicit range check |
| modulo arithmetic | signed wrap assumption | unsigned fixed-width with documented modular semantics |
| parsing untrusted lengths | unchecked multiplication | checked multiplication + failure path |

---

## 17. Key Takeaways

1. Signed overflow is UB in standard C++.
2. UB enables optimizer assumptions that can legally reshape control flow.
3. “Works on my compiler” is not a correctness argument.
4. Use precondition checks, wider intermediates, and explicit modular semantics where appropriate.
5. Enforce arithmetic safety via coding rules, tooling, and boundary-focused tests.

Arithmetic semantics must be designed, not guessed.
