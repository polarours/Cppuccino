# Fold Expression Errors

## Introduction

Fold expressions (C++17) let you "reduce" a parameter pack with an operator in one terse line:

```cpp
template <typename... Ts>
auto sum(Ts... ts) { return (ts + ...); }   // sum all arguments
```

They are wonderfully concise, but two mistakes are extremely common:

1. **Forgetting that an empty pack is ill-formed for most operators.** `(ts + ...)` with zero arguments is a compile error for `+`, `*`, `&`, etc. (only `&&`, `||`, and the comma operator have built-in empty-pack behavior).
2. **Misplacing parentheses / mixing up unary vs binary fold syntax**, producing "expected unqualified-id" or "pack must be expanded" errors.

## The Problem

```cpp
template <typename... Ts>
auto sum(Ts... ts) {
    return (ts + ...);   // ERROR when called as sum(): empty pack not allowed for +
}
```

```cpp
// Another frequent slip: wrong grouping turns a valid fold into a parse error
template <typename... Ts>
void print_all(std::ostream& os, Ts... ts) {
    (os << ... << ts);    // actually valid, but (os << ... << ts) is easy to
                          // mis-write as os << (... << ts), which is NOT a fold
}
```

**Why it's broken:**
- With no arguments, `(ts + ...)` has nothing to fold over. The standard says this is ill-formed for `+`, so `sum()` fails to compile even though the intent ("sum of nothing is 0") is natural.
- Fold syntax is strict: a *unary* fold is `(pack op ...)` or `(... op pack)`; a *binary* fold is `(pack op ... op init)` or `(init op ... op pack)`. Drop a parenthesis and the compiler cannot see a pack expansion, yielding a confusing message.

## The Fix

### 1. Provide an initializer so an empty pack is well-formed

```cpp
template <typename... Ts>
auto sum(Ts... ts) {
    return (ts + ... + 0);   // binary fold with init: empty pack -> 0, valid
}
```

Now `sum()`, `sum(1)`, and `sum(1, 2, 3)` all compile and behave correctly.

### 2. Use the correct parenthesization for stream/output folds

```cpp
template <typename... Ts>
void print_all(std::ostream& os, Ts... ts) {
    (os << ... << ts);   // correct unary fold over the pack, left-to-right
}
```

For a comma fold that evaluates each for side effects, wrap it: `((std::cout << ts << '\n'), ...);`.

## Best Practices

- **Always add an initializer** `(pack op ... op init)` when the function might be called with zero arguments and the operator has no natural empty-pack value (`+`, `*`, `&`, `|`, etc.). `&&`/`||`/`,` already have one, but being explicit is clearer.
- **Memorize the two shapes.** Unary: `(... op pack)` / `(pack op ...)`. Binary: `(pack op ... op init)`. The pack must be directly inside the parentheses.
- **Parentheses are the fold.** `(std::cout << ... << ts)` works; `std::cout << (... << ts)` does not — the whole fold must be wrapped.
- **Prefer a comma fold for side effects.** `((action(ts)), ...)` evaluates `action` for every element in order; it is the idiomatic "for each" with folds.
- **Watch associativity for non-associative operators.** Folds expand left-to-right for `(... op pack)`; if your operator is not associative (e.g. `-`), the result order matters — write it explicitly.

## Summary

Fold expressions are the cleanest way to reduce a parameter pack, but they demand respect for two rules: an empty pack is illegal for most operators (so supply an initializer), and the pack must sit correctly inside the fold's parentheses (unary vs binary shape). Get those right and folds replace pages of recursion with a single expressive line.
