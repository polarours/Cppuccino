# Forwarding References and Perfect Forwarding

## 1. Introduction

> **T&& Doesn’t Always Mean “Rvalue Reference”**  
> — Scott Meyers

In previous chapters we introduced **lvalue references** and **rvalue references**. This chapter goes further into **forwarding references** and why they are central to implementing **perfect forwarding**.

Before we start, let’s briefly explain the background of **forwarding references**:

- C++11 introduced rvalue references (`T&&`), but early on the syntax did not distinguish **forwarding references** from **ordinary rvalue references**. Many documents referred to all forms of `T&&` as rvalue references.
- This easily leads to a misconception: syntactically `T&&` looks like an **“rvalue reference”**, but during template type deduction its actual meaning is determined by the **deduction context**, not by the `&&` alone.
- To clarify this long-standing confusion, Scott Meyers coined **Universal Reference**, describing those `T&&` that behave differently depending on the argument type in a deduction context.

### 1.1 What Is a Forwarding Reference?

**A forwarding reference (also called a universal reference)** is not a new reference type. It is a semantic description of **the special binding behavior of `T&&` in template type deduction**. It can bind to both lvalues and rvalues while preserving the original value category. The canonical form is:

```cpp
template<typename T>
void func(T&& arg); // Here T&& is a forwarding reference (T is a template parameter)
```

> Only when `T` in `T&&` is a template parameter and appears in a template type deduction context is it a forwarding reference. If `T` is a concrete type (e.g., `int&&`), then it is just a normal rvalue reference.

### 1.2 Why Do We Need Forwarding References?

In generic programming, we often want to write a universal function or class template that forwards parameters **as-is** (including value category and const-qualification) to another function. Prior to forwarding references, C++’s traditional reference mechanism had clear limitations in achieving perfect forwarding. The table below compares reference types in generic scenarios and their drawbacks:

| Reference Type | Binding Capability | Limitation |
| --- | --- | --- |
| **Lvalue reference (`T&`)** | Only binds lvalues | Cannot accept temporaries (rvalues), so generic functions can’t handle rvalue arguments, reducing applicability. |
| **Const lvalue reference (`const T&`)** | Binds lvalues and rvalues | Forces parameters to become `const`, preventing non-`const` member calls and disabling move semantics (only copies). |
| **Rvalue reference (`T&&`)** | Only binds rvalues | Cannot accept ordinary lvalues, so generic functions can’t handle persistent objects. |

## 2. The Mechanics Behind Forwarding References

Forwarding references rely on two key mechanisms:

### 2.1 Reference Collapsing

Reference collapsing describes how reference types interact when references are themselves template parameters. The rules are simple:

| Reference Combination | Collapsed Result |
| --- | --- |
| `T& &` | `T&` |
| `T& &&` | `T&` |
| `T&& &` | `T&` |
| `T&& &&` | `T&&` |

> If any reference in the combination is an lvalue reference (`&`), the result collapses to an lvalue reference. Only `&&` + `&&` remains an rvalue reference.

### 2.2 Template Type Deduction

Template type deduction is the foundation of generic programming. When you call a template function without explicitly specifying its type parameters, the compiler runs a deduction process. For forwarding references, deduction depends on the argument’s value category. The table below shows the final deduction results:

| Argument | `T` | `T&&` |
| --- | --- | --- |
| Lvalue (e.g., `int x`) | `T = int&` | `T&& = int&` |
| Rvalue (e.g., `42`) | `T = int` | `T&& = int&&` |

> This is the fundamental reason why forwarding references can bind to both lvalues and rvalues.

## 3. Perfect Forwarding

The goal of **perfect forwarding** is to pass function parameters with the **exact same value category (lvalue/rvalue)** and `cv` qualifiers to another function, without introducing extra copies or losing move semantics.

Without perfect forwarding, template functions easily “accidentally turn rvalues into lvalues,” causing:

- Unnecessary copies
- Failure to call move-enabled overloads
- Code bloat (having to write multiple overloads)

> A simple way to think about it: **perfect forwarding = forwarding reference + `std::forward` + reference collapsing**.

### 3.1 What Does `std::forward` Do?

The C++ standard library provides `std::forward` to implement perfect forwarding. It conditionally casts the parameter back to its original value category. A simplified version is:

```cpp
template<typename T>
T&& forward(typename std::remove_reference<T>::type& param) noexcept {
	return static_cast<T&&>(param);
}
```

- If `T` is an lvalue reference type → `forward<T>` keeps it an lvalue
- If `T` is a non-reference type → `forward<T>` casts it to an rvalue (enabling moves)

### 3.2 Perfect Forwarding Example

```cpp
#include <iostream>
#include <utility>

void process(int& x) {
	std::cout << "Lvalue processed: " << x << std::endl;
}

void process(int&& x) {
	std::cout << "Rvalue processed: " << x << std::endl;
}

template<typename T>
void wrapper(T&& arg) {
	process(std::forward<T>(arg)); // Perfect forwarding
}

int main() {
	int a = 10;
	wrapper(a);        // Pass lvalue
	wrapper(20);       // Pass rvalue
	return 0;
}
```

Output:

```
Lvalue processed: 10
Rvalue processed: 20
```

Here `wrapper` uses a forwarding reference `T&& arg` and forwards it via `std::forward<T>(arg)` to `process`. Based on the original value category, the appropriate overload is selected.

### 3.3 Common Use Cases

#### 3.3.1 Case 1: Factory Functions / Constructor Forwarding

```cpp
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Usage
auto p1 = make_unique<std::string>("hello");                // rvalue
std::string s = "world";
auto p2 = make_unique<std::string>(s);                      // lvalue
auto p3 = make_unique<std::string>(std::move(s));           // explicit move
```

#### 3.3.2 Case 2: Container `emplace` / `insert` Forwarding

```cpp
template<typename T>
class MyContainer {
	std::vector<T> data;
public:
	template<typename... Args>
	void emplace_back(Args&&... args) {
		data.emplace_back(std::forward<Args>(args)...);
	}
};
```

#### 3.3.3 Case 3: Universal Proxy Functions

```cpp
template<typename Func, typename... Args>
auto invoke(Func&& f, Args&&... args) {
	return std::forward<Func>(f)(std::forward<Args>(args)...);
}
```

> These are just a few typical scenarios; in practice, perfect forwarding is widely used.

## 4. Notes and Pitfalls

Although forwarding references and perfect forwarding are core techniques in modern C++ generic programming, they are not a “use everywhere” tool. Misuse often introduces subtle and hard-to-debug problems.

This section discusses typical pitfalls and reasonable boundaries from both language semantics and engineering design perspectives.

### 4.1 Forwarding References Are Not “Universal Parameters”

A common misconception is:

> “If it’s a template function parameter, just write it as `T&&`.”

That is not correct. Consider:

```cpp
template<typename T>
void setValue(T&& value);
```

It seems to accept both lvalues and rvalues, but that can be precisely the problem.

Because `T&&` is a forwarding reference, this interface will match almost any argument form during deduction, causing:

- Lvalues, rvalues, and const lvalues to all end up in the same template overload.
- Unstable interface semantics: the same function may sometimes “borrow” an object and sometimes “take ownership,” but that difference is not reflected in the API design.

### 4.2 Limit Forwarding References to Implementation Details

Forwarding references have extremely high matching priority, which can significantly impact overload resolution:

```cpp
template<typename T>
void func(T&& arg);   // forwarding reference

void func(int& arg);  // ordinary lvalue reference
void func(int&& arg); // ordinary rvalue reference

int x = 10;
func(x);              // calls forwarding-reference version
func(20);             // calls forwarding-reference version
```

In this case:

- `func(x)` and `func(20)` both may match the template version.
- This makes overload resolution less clear than intended.

That’s why in the standard library:

- Public interfaces rarely expose forwarding references directly.
- Forwarding references are typically confined to implementation details.

### 4.3 `const` Restricts Perfect Forwarding

```cpp
template<typename T>
void wrapper(T&& arg);
```

If the argument is `const`:

```cpp
const int x = 10;
wrapper(x);
```

Then:

- `T` is deduced as `const int&`
- `T&&` collapses to `const int&`

Result:

- Even with `std::forward<T>(arg)`, the forwarded argument is still `const int&`
- Non-`const` overloads cannot be called

> There are many more caveats. The takeaway is that forwarding references and perfect forwarding are powerful tools, but they must be used carefully.

## 5. Summary

This chapter has systematically covered **forwarding references** and **perfect forwarding**, from language design and standard semantics to engineering practice.

We saw that:

- A forwarding reference is not a new reference type, but a semantic phenomenon produced by **template type deduction + reference collapsing**.
- Perfect forwarding restores an argument’s original value category and `cv` qualifiers in a template context.
- `std::forward` does not “create” move semantics; it merely **permits** move semantics when conditions are met.

More importantly, we emphasized the **boundaries of use**:

- It is not the default choice for public interfaces.
- Its high matching power can interfere with overload resolution.
- Misuse can introduce unstable semantics and hidden defects.

Understanding these constraints is the prerequisite for using forwarding references correctly.
