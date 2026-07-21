# Template Argument Deduction Pitfalls

## The Problem

Template argument deduction can fail or produce unexpected results,
especially with:
- Reference collapsing rules
- Universal references vs rvalue references
- Non-deduced contexts

## Examples

```cpp
// Reference collapsing surprise
template <typename T>
void f(T&& arg) {}  // Forwarding reference, NOT rvalue reference

int x = 42;
f(x);      // T = int&, arg is int& (lvalue reference)
f(42);     // T = int, arg is int&& (rvalue reference)
```

```cpp
// Non-deduced context
template <typename T>
void g(std::vector<T> v) {}  // T deduced from vector element type

std::vector<int> v = {1, 2, 3};
g(v);     // OK: T = int

// But this fails:
g({1, 2, 3});  // ERROR: cannot deduce T from initializer list
```

```cpp
// Deduction guides confusion
std::pair p(1, 2.0);  // C++17: T = pair<int, double>
// But:
std::pair p{1, 2.0};  // T = pair<double, double> on some compilers!
```

```cpp
// SFINAE doesn't work everywhere
template <typename T>
auto toString(T value) -> decltype(std::to_string(value)) {
    return std::to_string(value);
}

// This fails for types without std::to_string overload
// toString(ComplexNumber{})  // ERROR: no matching overload
```

## Prevention

- Understand reference collapsing rules (T& + & = T&, T& + && = T&)
- Use `std::decay_t` when you want value types
- Prefer explicit template arguments for ambiguous cases
- Use `std::enable_if` or concepts for SFINAE constraints
- Test deduction with `static_assert` or `typeid`

## See Also

- [Implicit Conversions](implicit-conversions.md)
- [Object Lifetime](object-lifetime-and-destruction-order-pitfalls.md)
