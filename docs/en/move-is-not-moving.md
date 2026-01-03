# std::move does not "move" anything

## Core idea

`std::move` is not an "action", but a "promise".
By itself, it:
- Does not copy
- Does not release
- Does not move
- Does not modify

The only thing it does is:
> Unconditionally converts an expression to an rvalue reference, thereby explicitly indicating to the compiler and readers:
> The ownership of the object can be transferred, and its current state no longer needs to be preserved.

Whether anything is actually moved depends entirely on:
- The object's type
- Whether a usable move constructor / move assignment exists
- Whether the current context allows calling them

`std::move` just opens the door; it does not carry anything out.

> Tip: For more about rvalue references, see the [Rvalue References](/docs/en/rvalue-references.md) chapter.

## A common but dangerous misunderstanding

Many people semantically misunderstand as follows:

```cpp
std::move(a); // a has been moved away
```

In fact, `std::move` does not change the state or content of `a`.
The actual resource transfer happens in the subsequently called constructor or assignment operator, not in `std::move` itself.

That is to say:
> Moving happens at the "construction / assignment" stage, not at the `std::move` line.

## What does std::move actually do?
Semantically, `std::move` is very simple, similar to this:

```cpp
template <typename T>
typename std::remove_reference<T>::type&& move(T&& t) noexcept {
	return static_cast<typename std::remove_reference<T>::type&&>(t);
}
```

That means:
- It does not care about the object's current state
- It simply forces the parameter `t` to an rvalue reference type
- It guarantees no resource transfer or release
    
It is just telling the compiler: "You may treat this lvalue as an object I'm about to abandon."

## Why does C++ need std::move?

To understand `std::move`, the key is not "how to use it", but "why it must exist".

In C++:
- "Variable names" are lvalues (lvalue) by default
- Even if a variable is no longer needed, it remains an lvalue unless explicitly cast otherwise

What if `std::move` is not available?
Therefore, the compiler cannot distinguish between "objects that should still be used" and "objects that can be safely discarded".

This leads to:
- Unable to call move constructors or move assignments, resulting in unnecessary copies and performance loss.
- Only the copy constructors or copy assignments are called, which may not be efficient for large objects.

## `std::move` and move constructors: the actual place where moving happens
```cpp 
std::string str = "Hello, World!";
std::string movedStr = std::move(str); // Calls std::string's move constructor
```

What happens?
1. `std::move(str)` casts `str` to `std::string&&` (an rvalue reference).
2. Because `std::string` has a move constructor, the compiler chooses it and moves `str`'s resources into `movedStr`.
3. `str` is now in a valid but unspecified state (typically empty).

Moving is not a language behavior; it is a type behavior.

## Object state after move: Why "valid but unspecified state"
After using `std::move`, the source object's state depends on that type's move semantics.

- For standard library types (like `std::string`, `std::vector`, etc.), moving typically transfers ownership of resources to the new object; the source becomes empty or "valid but unspecified."
- For user-defined types, the post-move state depends entirely on how you implement the move constructor and move assignment.

Example:
```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};
std::vector<int> movedVec = std::move(vec);
// vec remains valid, but its contents may have been moved (often empty).
```
> Tip: For more details on the move process and ownership, see later chapters (to be added).

---

## Important notes

- `std::move` itself does not change an object's state; it only allows move semantics. The original object remains valid but may be empty or unspecified, depending on the type's move implementation.
- Use `std::move` only when you truly need to move resources to avoid unnecessary complexity and potential bugs.
- It is a casting utility; it does not automatically free or transfer resources.
