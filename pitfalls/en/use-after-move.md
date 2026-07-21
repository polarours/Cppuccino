# Use After Move

## The Problem

Moving from an object leaves it in a valid but unspecified state.
Using the moved-from object without reinitializing leads to undefined behavior
or unexpected results.

## Examples

```cpp
std::string s1 = "Hello";
std::string s2 = std::move(s1);

// s1 is now in a valid but unspecified state
// s2 = "Hello", s1 might be empty or might still have data
std::cout << s1.size();  // UB or unpredictable
```

```cpp
// Common mistake with containers
std::vector<int> v1 = {1, 2, 3};
std::vector<int> v2 = std::move(v1);

// After move, v1 is empty on most implementations
// but the standard only guarantees "valid but unspecified"
if (!v1.empty()) {
    // Might execute on some implementations!
    process(v1);
}
```

## Why It Matters

The moved-from state is implementation-defined. Relying on specific
post-move values is non-portable and a source of hard-to-find bugs.

## Prevention

- Never use a moved-from object without reassignment
- Prefer `std::exchange(obj, {})` when you need the value and want to move
- Use clear variable names: `original` vs `moved_to`
- After move, treat the object as "empty" or "reset"

## See Also

- [Dangling Pointer](dangling-pointer.md)
- [Object Lifetime](object-lifetime-and-destruction-order-pitfalls.md)
