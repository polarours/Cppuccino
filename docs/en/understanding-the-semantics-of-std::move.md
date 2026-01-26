# Deep Understanding of `std::move` Semantics  

## Core Point: move ≠ move

`std::move` explicitly declares: 
the object no longer bears its original semantic responsibilities from this point forward.

It itself:
- Does not copy
- Does not release
- Does not move
- Does not modify

What it does solely is:
> Unconditionally convert an expression to an rvalue reference, thereby explicitly expressing to both compiler and reader:
> The ownership of this object can be transferred, and its current state no longer needs to be preserved.

Whether actual "movement" occurs depends entirely on:
- The object's type
- Whether available move constructors / move assignments exist
- Whether the current context allows calling them

> **Note**: For detailed explanations of lvalue references and rvalue references, please refer to the [Lvalue References and Rvalue References](/docs/en/lvalue-and-rvalue-references.md) section.

## A Common But Dangerous Misconception

Many people have the following semantic misconception:

```cpp
std::move(a); // a has been moved away
```

Actually, `std::move` does not change the state or content of `a`.
The actual resource transfer happens in the subsequent constructor or assignment operator, not in the `std::move` itself.

In other words:

> Movement occurs during the "construction/assignment" phase, not on the line with `std::move`.

## So What Does std::move Actually Do?

Semantically, the implementation of `std::move` is quite simple, similar to the code below:

```cpp
template<class T>
constexpr std::remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
}
```

> It is equivalent to a `static_cast<T&&>` (more precisely, cast to a "right value reference of T after removing reference").
> Note: Using `std::move` with `const T` generally does not trigger move construction, since move constructors typically cannot accept `const T&&`.

That is to say:
- Doesn't care whether the object is about to be destroyed
- Doesn't check the object's current state
- Doesn't trigger any resource management logic
- Performs only type-level conversion

> It merely tells the compiler: "I allow you to treat the current lvalue as an object about to be abandoned."

## Why Does C++ Need std::move?

Understanding `std::move`, the key is not in "how to use it," but in why it must exist.

In C++:
- "Variable names" are lvalues (lvalue) by default
- Even if a variable is about to be destroyed, it remains an lvalue in expressions

To change this behavior, C++11 introduced rvalue references, but only through conversions—specifically move semantics—can an lvalue be turned into an rvalue. This is the reason `std::move` exists.

## std::move and Move Construction: Where Real Movement Happens

Let's look at the following code:
```cpp 
std::string str = "Hello, World!";
std::string movedStr = std::move(str); // Here calls std::string's move constructor
```

What happened in between?
1. `std::move(str)` converts `str` to `std::string&&` (rvalue reference).
2. Since `std::string` has a move constructor, the compiler chooses to call it, moving the resources of `str` to `movedStr`.
3. `str` is now in a valid but unspecified state (usually an empty string).

## Objects After move: Why They Are in "Valid but Unspecified State"

After using `std::move`, the state of the original object depends on its type's move semantics:
- For standard library types (such as `std::string`, `std::vector`, etc.), after moving, resources are usually transferred to the new object, leaving the original empty or in some "valid but unspecified" state.
- For custom types, the post-move state completely depends on how you implement move constructors and move assignment operators.

For example:
```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};
std::vector<int> movedVec = std::move(vec);
// vec is still valid, but content may have been moved (typically becomes empty).
```
> **Note**: For more detailed introduction of the `move` process and ownership, please refer to upcoming sections (to be added).

## Summary

Let's summarize the key points of `std::move`:
- `std::move` itself does not change the object's state; it only allows move semantics to happen. The original object is still valid, but the state may be unspecified or empty, depending on the type's move implementation.
- Only use `std::move` when you actually need to move resources, to avoid unnecessary complexity and potential errors.
- It is a type conversion tool that does not automatically release or transfer resources.