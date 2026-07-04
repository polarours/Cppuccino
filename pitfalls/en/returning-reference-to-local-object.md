# Returning a Reference (or Pointer) to a Local Object

## 0. Scope and Conclusions First

This article discusses one of the most common C++ lifetime bugs:

- returning a reference to an automatic local object,
- returning a pointer to an automatic local object,
- returning views/iterators that indirectly refer to automatic local objects.

Core conclusion:

1. If a function returns, all of its automatic local objects are already destroyed.
2. Any reference/pointer/view that still points to those objects is dangling.
3. Reading or writing through a dangling handle is undefined behavior (UB).

Engineering conclusion:

- Prefer return-by-value.
- Return references only when the referenced object is guaranteed to outlive all uses.
- Make ownership and borrowing contracts explicit at API boundaries.

---

## 1. Minimal Failing Case

```cpp
const std::string& bad() {
    std::string s = "cpp";
    return s; // always dangling after return
}
```

What is wrong:

- `s` has automatic storage duration.
- Its lifetime ends when `bad()` exits.
- The returned reference refers to a non-existent object.

The same bug shape exists for pointers:

```cpp
const std::string* bad_ptr() {
    std::string s = "cpp";
    return &s; // dangling pointer
}
```

And for indirect handles:

```cpp
std::string_view bad_view() {
    std::string s = "cpp";
    return std::string_view{s}; // view to dead buffer
}
```

---

## 2. Why This Is UB: Storage Duration vs Object Lifetime

You must separate two concepts:

1. **Storage duration** (where memory comes from and how long storage exists).
2. **Object lifetime** (when an object begins and ends as a valid C++ object).

For automatic locals:

- storage is tied to function scope,
- lifetime ends when scope exits,
- destructor runs before function returns to caller.

So the caller never observes a valid local object through a returned reference.

In formal terms, once lifetime ends, any glvalue/pointer designating that object no longer denotes a valid object for access. Using it is UB.

---

## 3. “But It Seems to Work on My Machine”

This bug is often deceptive because:

1. Stack memory may still contain old bytes briefly.
2. Optimizations may reorder code.
3. Different builds produce different symptoms.

Possible outcomes:

- apparently correct output,
- garbage values,
- intermittent crashes,
- silent data corruption.

All of these are consistent with UB.

If behavior looks stable in debug and fails in release, that is a classic UB signature.

---

## 4. Common Variants in Real Code

### 4.1 Returning reference to local container element

```cpp
const int& bad_elem() {
    std::vector<int> v{1, 2, 3};
    return v[0]; // element dies with v
}
```

### 4.2 Returning C-string pointer from local `std::string`

```cpp
const char* bad_cstr() {
    std::string s = "hello";
    return s.c_str(); // pointer invalid after return
}
```

### 4.3 Returning iterator into local container

```cpp
std::vector<int>::const_iterator bad_it() {
    std::vector<int> v{1, 2, 3};
    return v.begin(); // iterator dangling
}
```

### 4.4 Returning `std::span` into local array

```cpp
std::span<const int> bad_span() {
    int a[] = {1, 2, 3};
    return std::span<const int>(a); // span points to dead array
}
```

### 4.5 Returning lambda capturing local by reference

```cpp
auto bad_lambda() {
    int x = 42;
    return [&]() { return x; }; // closure stores dangling reference
}
```

---

## 5. Correct Patterns

## 5.1 Return by value (default choice)

```cpp
std::string good() {
    std::string s = "cpp";
    return s; // NRVO/move; safe and efficient
}
```

Why this is preferred:

- clear ownership transfer,
- no dangling borrow,
- modern compilers optimize copies aggressively.

### 5.2 Return reference to stable owner object

```cpp
class Config {
public:
    const std::string& name() const noexcept { return name_; }
private:
    std::string name_ = "service";
};
```

This is safe because:

- reference points to `name_` inside `*this`,
- caller must ensure `Config` object remains alive.

### 5.3 Return smart pointer for shared ownership semantics

```cpp
std::shared_ptr<std::string> make_name() {
    return std::make_shared<std::string>("cpp");
}
```

Use when ownership is intentionally shared.

### 5.4 Caller-supplied output object

```cpp
void fill_name(std::string& out) {
    out = "cpp";
}
```

Good for hot paths when you want explicit reuse of caller storage.

---

## 6. Lifetime-Safe API Design Rules

At API boundaries, make contracts explicit:

1. **Owned return**: return value/smart pointer.
2. **Borrowed return**: return reference/view only if source owner is explicit and stable.
3. **No hidden owner**: avoid returning borrow tied to hidden temporaries.

Recommended naming/documentation style:

- `get_*_copy()` for owned values,
- `*_view()` for borrowing views,
- mention precondition: “returned view valid while `X` remains alive and unmodified”.

---

## 7. What About `static` Locals?

This is a distinct case:

```cpp
const std::string& singleton_name() {
    static const std::string s = "cpp";
    return s; // lifetime: program duration
}
```

This avoids dangling because static local lifetime is program-long.

But evaluate trade-offs:

- hidden global state,
- initialization/destruction ordering concerns across translation units,
- potential testability costs.

Use intentionally, not as a blanket workaround.

---

## 8. Return-by-Value Cost Myth (Modern C++)

Many buggy APIs exist because developers overestimated copy cost.

In modern C++:

- copy elision is common,
- move construction is cheap for many standard/library types,
- value semantics greatly simplify correctness.

Correctness-first rule:

$$
	ext{Safe lifetime contract} > \text{micro-optimization based on assumption}
$$

If profiling later proves a hotspot, optimize with measured evidence.

---

## 9. Review Checklist (Code Review / PR)

When reviewing return statements, ask:

1. Is return type a reference/pointer/view/iterator?
2. Does it refer to automatic local state?
3. Does it refer to temporary subobject/buffer?
4. Is owner visible and guaranteed alive in caller context?
5. Would return-by-value remove ambiguity?

Red flags:

- `return local;` where function returns `T&` or `const T&`.
- `return &local;`.
- `return local.c_str();`.
- `return std::string_view(local);`.
- returning lambda with `[&]` capture from function.

---

## 10. Tooling and Diagnostics

Useful defenses:

1. Compiler warnings:
   - enable high warning levels,
   - treat lifetime warnings as errors in CI.
2. Sanitizers:
   - AddressSanitizer can catch many dangling accesses.
3. Static analysis:
   - lifetime and escape analysis rules,
   - custom checks for return of locals by reference/pointer.

No tool is complete; combine tools with strong API design.

---

## 11. Migration Strategy for Legacy Code

If legacy APIs already return borrowed references dangerously:

1. Classify each function:
   - always dangling,
   - conditionally safe,
   - safe by ownership contract.
2. Introduce safe replacements:
   - value-return overloads,
   - explicit owner parameters.
3. Deprecate unsafe signatures.
4. Incrementally migrate call sites.
5. Add regression tests under sanitizers.

Prefer staged migration over big-bang rewrites.

---

## 12. Bad vs Good Patterns (Compact Table)

| Pattern | Safety | Better Choice |
|---|---|---|
| return `T&` to local `T` | UB | return `T` |
| return `T*` to local `T` | UB | return `T` or owner pointer |
| return `string_view` to local `string` | UB | return `string` |
| return iterator to local container | UB | return container/value |
| return ref to member of stable object | potentially safe | document owner-lifetime contract |

---

## 13. Key Takeaways

1. Local automatic objects die at function exit.
2. Returning references/pointers/views to them creates dangling handles.
3. Access through dangling handles is UB.
4. Return-by-value is the default robust interface in modern C++.
5. If borrowing is required, make owner and lifetime constraints explicit and verifiable.

Correctness of lifetime contracts is not optional API polish; it is a foundational safety property.
