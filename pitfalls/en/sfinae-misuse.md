# SFINAE Misuse

## Introduction

SFINAE — *Substitution Failure Is Not An Error* — is the rule that an invalid type substitution while forming an overload's signature simply **removes that overload** from the candidate set, rather than failing the whole compilation. It is the classic mechanism for "enable this overload only when the type supports X."

The pitfall is that SFINAE **only applies in the immediate context of the template's declaration** (its parameter list, return type, or template arguments). If your constraint lives in the function *body* or a place outside that immediate context, a bad substitution becomes a **hard compile error** instead of a quiet removal.

## The Problem

```cpp
#include <vector>

// WRONG: the requirement is checked inside the body.
// If T has no .size(), this is a HARD error, not SFINAE.
template <typename T>
void call_size(const T& t) {
    t.size();           // compile error when T lacks .size()
    // (and it errors even before overload resolution can "discard" this template)
}
```

A second common misuse: writing a constraint that is checked too late, so the template is selected and *then* fails deep inside, producing a cryptic error far from the real cause.

**Why it's broken:**
- SFINAE only rescues substitutions that happen while deducing the function's signature. A call like `t.size()` in the body is ordinary code; if `T` has no `size()`, the compiler reports a normal "no member named 'size'" error — it cannot "un-pick" this overload.
- The result is a long, confusing error message, and the intended fallback overload never gets a chance to be chosen.

## The Fix

Move the constraint into the signature so the failure is a substitution failure (soft), not a body error (hard). A small trait plus `std::enable_if` is the canonical form:

```cpp
#include <iostream>
#include <type_traits>
#include <vector>

template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>>
    : std::true_type {};

// Constraint lives in the signature -> SFINAE-safe
template <typename T>
std::enable_if_t<has_size<T>::value, std::size_t>
call_size(const T& t) {
    return t.size();    // now guaranteed that T has .size()
}

int main() {
    std::vector<int> v{1, 2, 3};
    return call_size(v) == 3 ? 0 : 1;   // selects the overload via SFINAE
    // call_size(42);  // simply does NOT match -> no hard error
}
```

For C++17 and later, `if constexpr` or concepts (`requires` / `std::enable_if` alternatives) are usually clearer and give better diagnostics — but the core rule is identical: **check in the signature, not the body.**

## Best Practices

- **Put constraints in the signature** (return type, template parameter, or — best — a `requires` clause in C++20). Never rely on the body to "reject" a type.
- **Prefer concepts (C++20) over hand-rolled SFINAE** when available: `template <typename T> requires requires(T t){ t.size(); }` is far more readable and produces better errors.
- **Use `std::void_t` traits** to probe for members/types; they are the standard SFINAE idiom and compose well.
- **Watch the immediate-context boundary.** If your "constraint" can be moved into `decltype(...)` in the return type or a default template argument, do so — that is what makes it soft instead of hard.
- **Don't over-constrain.** Each `enable_if` narrows the overload set; two overloads with overlapping conditions become ambiguous, so keep conditions mutually exclusive.

## Summary

SFINAE is a safety valve, not a debugging tool: it turns an invalid substitution into a quiet "this overload is not a candidate." The misuse that breaks it is checking the requirement in the function body, where failure is a hard error. Keep constraints in the signature (traits + `enable_if`, or `requires` in C++20) and SFINAE does its job; put them in the body and you get a cryptic compile failure instead.
