[中文](move-is-not-moving_zh-CN.md) | [English](move-is-not-moving.md)

# std::move does not "move" anything

## Core idea

`std::move` itself **does not perform a move operation**. Its only job is to unconditionally cast an expression to an **rvalue reference**, which **allows** later code to choose move semantics.

Whether anything is actually moved depends entirely on:
- The object's type
- Whether a usable move constructor / move assignment exists
- Whether the current context allows calling them

`std::move` just opens the door; it does not carry anything out.

> Tip: For more about rvalue references, see the related chapter (to be added).

## Common misunderstanding

Many people (including me when I started) instinctively think:

```cpp
std::move(a); // a has been moved away
```

In reality, `std::move` does not change `a`'s state or contents. The real move happens in the move constructor or move assignment operator, not in `std::move` itself.

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

## Example

```cpp 
std::string str = "Hello, World!";
std::string movedStr = std::move(str); // Calls std::string's move constructor
```

What happens?
1. `std::move(str)` casts `str` to `std::string&&` (an rvalue reference).
2. Because `std::string` has a move constructor, the compiler chooses it and moves `str`'s resources into `movedStr`.
3. `str` is now in a valid but unspecified state (typically empty).

Moving is not a language behavior; it is a type behavior.

## Object state after move
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
