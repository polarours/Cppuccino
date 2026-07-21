# Const Correctness Pitfalls

## The Problem

Const correctness in C++ has subtle issues, especially with:
- const references to temporaries
- const member functions and mutable state
- casting away const

## Examples

```cpp
// const reference extends temporary lifetime
const std::string& ref = std::string("temporary");
// ref is valid here! The temporary lives as long as ref

// But be careful with pointers
const char* ptr = std::string("temporary").c_str();
// DANGER: ptr may point to destroyed memory!
// c_str() returns a pointer, not a reference
```

```cpp
class Cache {
public:
    int get(int key) const {  // const member function
        if (auto it = cache_.find(key); it != cache_.end()) {
            return it->second;  // OK: reading
        }
        int value = compute(key);
        cache_[key] = value;    // ERROR: modifying in const function
        return value;
    }
private:
    mutable std::map<int, int> cache_;  // mutable fixes this
    int compute(int key) const;
};
```

```cpp
// Casting away const is dangerous
const int x = 42;
int* ptr = const_cast<int*>(&x);
*ptr = 100;  // UNDEFINED BEHAVIOR (x is const)

// Only valid for:
// 1. Returning non-const from a const function
// 2. Removing const from originally non-const objects
```

## Prevention

- Prefer const references over const pointers
- Use `mutable` for cache/memoization in const functions
- Never const_cast to modify a truly const object
- Build with `-Wcast-qual` to catch unsafe casts

## See Also

- [Implicit Conversions](implicit-conversions.md)
- [Undefined Behavior](../docs/en/undefined-behavior.md)
