# Template Instantiation and Two-Phase Lookup

## 1. Introduction

Template programming in C++ is powerful because code is checked and specialized in context.
That power also introduces a frequent source of confusion:

> template definitions are analyzed before concrete types are known,
> then re-analyzed at instantiation for dependent parts.

This model is commonly called **two-phase lookup**.
If developers conflate the two phases,
they get fragile generic code, delayed diagnostics, and portability problems.

This chapter focuses on practical correctness:

1. what is checked at definition time,
2. what is deferred to instantiation,
3. how to structure templates for clear diagnostics.

---

## 2. Scope and Non-Goals

### 2.1 Scope

We cover:

- dependent vs non-dependent names,
- `typename` and `template` disambiguators,
- argument-dependent lookup interactions,
- constraints (`concepts` / `requires` / `static_assert`),
- engineering patterns for diagnostics and maintainability.

### 2.2 Non-Goals

We do not provide a complete language-lawyer treatment of every corner case.
The objective is robust engineering reasoning.

---

## 3. The Two Phases, Precisely

### 3.1 Definition Phase

When a template is parsed/defined:

1. non-dependent names are resolved,
2. syntax and immediate context checks run,
3. dependent constructs are preserved for later substitution.

### 3.2 Instantiation Phase

When template arguments are substituted:

1. dependent names are looked up,
2. dependent expressions are type-checked,
3. substitution may fail (potentially via SFINAE or hard error).

A large portion of template diagnostics originates here,
often far from the template definition site.

---

## 4. Dependent vs Non-Dependent Names

### 4.1 Non-Dependent Name

A name that does not depend on template parameters.
It must be valid at template definition point.

### 4.2 Dependent Name

A name whose meaning depends on template parameters.
It is resolved at instantiation.

Example:

```cpp
template <typename T>
void print_size() {
    std::size_t n = T::size();
    (void)n;
}
```

- `std::size_t` is non-dependent.
- `T::size` is dependent.

---

## 5. Why `typename` Exists

Dependent qualified names are ambiguous to parser:

```cpp
template <typename T>
void f() {
    // typename required: T::value_type is a dependent type
    typename T::value_type x{};
    (void)x;
}
```

Without `typename`, compiler cannot assume a dependent qualified name denotes a type.

Rule of thumb:

- for dependent qualified type names, write `typename` unless grammar forbids it.

---

## 6. Why `template` Disambiguator Exists

When calling a dependent template member,
parser needs explicit cue that `<...>` starts template arguments.

```cpp
template <typename T>
void g(T& t) {
    t.template apply<int>(42);
}
```

Without `template`, `<` may be parsed as less-than operator.

---

## 7. Base Classes and Dependent Lookup Pitfalls

A frequent issue appears with dependent base classes:

```cpp
template <typename T>
struct Base {
    void foo() {}
};

template <typename T>
struct Derived : Base<T> {
    void bar() {
        // foo(); // often fails lookup as non-dependent in this context
        this->foo(); // correct: makes name dependent
    }
};
```

Use `this->` or `Base<T>::` to make intent explicit.

---

## 8. ADL and Customization Points

Argument-dependent lookup (ADL) interacts with templates subtly.

Common generic pattern:

```cpp
using std::swap;
swap(a, b); // enables ADL + std fallback
```

If you write `std::swap(a, b)` unconditionally,
you can disable useful user-defined overload participation.

Two-phase lookup and ADL together define which candidate set is visible.

---

## 9. Diagnostics Delay: Why Errors Look Remote

Template definition may compile cleanly,
yet fail only when instantiated with specific type.

This is expected behavior, not compiler inconsistency.

Engineering implication:

1. keep template bodies small,
2. constrain early,
3. isolate dependent-heavy expressions.

---

## 10. Constraining Templates Early (C++20)

Concepts allow clear, local constraints:

```cpp
#include <concepts>

template <typename T>
concept HasSize = requires { T::size(); };

template <HasSize T>
void print_size2() {
    auto n = T::size();
    (void)n;
}
```

This shifts many errors from deep instantiation traces to direct constraint diagnostics.

---

## 11. Pre-C++20 Constraint Style

For older standards, use `static_assert` with traits/detection idioms.

```cpp
template <typename T>
void h() {
    static_assert(sizeof(T) > 0, "T must be complete");
}
```

While less expressive than concepts,
it still improves diagnostic intent.

---

## 12. SFINAE vs Hard Errors

Substitution failure in immediate context may remove overload candidates (SFINAE).
But failures outside that context often become hard errors.

Practical advice:

1. prefer explicit constraints over deep SFINAE tricks,
2. avoid relying on accidental substitution behavior for API selection.

---

## 13. Keep Dependent Logic Local

Monolithic template bodies generate unreadable diagnostics.

Better structure:

1. public constrained facade,
2. small helper templates,
3. isolated trait checks.

This yields cleaner compile errors and easier maintenance.

---

## 14. Example: Layered Design

```cpp
template <typename T>
concept Streamable = requires(std::ostream& os, const T& v) {
    { os << v } -> std::same_as<std::ostream&>;
};

template <Streamable T>
void dump(const T& v) {
    dump_impl(v);
}

template <typename T>
void dump_impl(const T& v) {
    std::cout << v << '\n';
}
```

Constraint failure is reported at `dump`, not buried in internals.

---

## 15. Hidden Friend and Lookup Strategy

“Hidden friend” functions declared inside class templates participate in ADL.
This is often used for operators.

It is powerful, but can confuse readers if overused.
Document intended customization model clearly.

---

## 16. Portability Considerations

Historically, compiler support for strict two-phase lookup differed.
Modern compilers are far better aligned,
but code relying on non-standard lookup behavior can still break.

Portability guideline:

1. write standards-conforming dependent name qualifiers,
2. avoid compiler-extension-dependent lookup tricks,
3. test across toolchains early.

---

## 17. Common Anti-Patterns

1. Missing `typename` on dependent type names.
2. Missing `template` disambiguator on dependent template member calls.
3. Unqualified calls that accidentally rely on non-portable lookup behavior.
4. Deep unconstrained templates producing unreadable diagnostics.
5. Overusing meta-tricks where concepts are clearer.

---

## 18. Debugging Workflow for Lookup Errors

When template lookup errors appear:

1. classify failing name as dependent or non-dependent,
2. add required `typename` / `template`,
3. check dependent base access (`this->` / `Base<T>::`),
4. add/strengthen constraints,
5. reduce reproducer to minimal template.

This systematic approach prevents blind trial-and-error edits.

---

## 19. Code Review Checklist

- [ ] Are dependent type names properly prefixed with `typename`?
- [ ] Are dependent template member calls prefixed with `template` where required?
- [ ] Is dependent base member access explicit (`this->` or qualified base)?
- [ ] Are constraints explicit and close to API boundary?
- [ ] Are diagnostics likely to be understandable for users?
- [ ] Is ADL behavior intentional and documented?

If not, refactor before adding more template complexity.

---

## 20. Design Guidance Summary

1. Keep template interfaces constrained and explicit.
2. Keep implementation fragments small and composable.
3. Treat two-phase lookup as correctness model, not compiler quirk.
4. Prefer clarity over meta-programming cleverness.
5. Fail early with concepts/static_assert instead of deep instantiation explosions.

---

## 21. Worked Mini-Case

```cpp
template <typename T>
struct Adapter : T {
    void call() {
        // this-> required for dependent base member lookup
        this->run();
    }
};
```

Without `this->`, many compilers emit lookup errors under strict rules.
This tiny example explains a large class of real-world template bugs.

---

## 22. Conclusion

Two-phase lookup is not optional complexity.
It is the mechanism that keeps template checking coherent across generic and concrete contexts.

High-quality template code follows three principles:

1. explicit dependence handling (`typename`, `template`, `this->`),
2. explicit constraints,
3. small, diagnosable template units.

When these are respected,
generic libraries become more portable, more diagnosable, and significantly easier to evolve.
