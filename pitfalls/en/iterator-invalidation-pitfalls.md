# Iterator Invalidation Pitfalls

## Problem Description

Iterator invalidation is one of the most common C++ traps. When a container is
modified, iterators you already hold may become invalid; using them afterwards
is undefined behavior.

## Common Scenarios

### 1. vector iterator invalidation

```cpp
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    auto it = v.begin();

    // Danger: insert may reallocate
    v.insert(v.begin(), 0);

    // Undefined behavior: it may already be invalid
    std::cout << *it << "\n";  // BUG!

    return 0;
}
```

**Why**: `vector::insert` reallocates when there is not enough capacity, and
every iterator, pointer, and reference into the vector dangles.

### 2. map/set iterator invalidation

```cpp
#include <map>
#include <iostream>

int main() {
    std::map<int, std::string> m = {{1, "one"}, {2, "two"}};

    auto it = m.begin();

    // Danger: erase invalidates it
    m.erase(it++);  // correct: increment first, then erase the old iterator

    // Danger: erasing directly and reusing the iterator
    it = m.begin();
    m.erase(it);
    std::cout << it->second << "\n";  // BUG! it is invalid now
}
```

**Why**: `std::map::erase(it)` invalidates `it` but returns the next valid
iterator (since C++11).

## Safe Patterns

### Safe vector operations

```cpp
#include <vector>
#include <algorithm>

bool shouldErase(int x) { return x % 2 == 0; }

void demo() {
    std::vector<int> v = {1, 2, 3};

    // Option 1: use indices
    for (size_t i = 0; i < v.size(); ++i) {
        // safe to use v[i]
    }

    // Option 2: update the iterator from erase's return value
    for (auto it = v.begin(); it != v.end(); ) {
        if (shouldErase(*it)) {
            it = v.erase(it);  // erase returns the next valid iterator
        } else {
            ++it;
        }
    }

    // Option 3: erase-remove idiom
    v.erase(std::remove_if(v.begin(), v.end(), shouldErase), v.end());
}
```

### Safe map/set operations

```cpp
#include <map>
#include <string>

bool shouldErase(int key) { return key % 2 == 0; }

void demo() {
    std::map<int, std::string> m = {{1, "one"}, {2, "two"}};

    // update the iterator from erase's return value
    for (auto it = m.begin(); it != m.end(); ) {
        if (shouldErase(it->first)) {
            it = m.erase(it);  // valid since C++11
        } else {
            ++it;
        }
    }
}
```

## Container Behavior Summary

| Operation | vector | string | deque | list | forward_list | map/set | unordered_map/set |
|-----------|--------|--------|-------|------|--------------|---------|-------------------|
| insert (not at end) | all invalid | all invalid | all invalid | unchanged | unchanged | unchanged | unchanged |
| insert (at end) | all invalid | all invalid | unchanged | unchanged | unchanged | unchanged | unchanged |
| erase (at position) | invalidated after position | invalidated after position | invalidated after position | unchanged | unchanged | returns next | returns next |
| clear | all invalid | all invalid | all invalid | unchanged | unchanged | unchanged | unchanged |
| resize | all invalid | all invalid | unchanged | unchanged | unchanged | unchanged | unchanged |

## Best Practices

1. **Avoid mutating container size inside a loop**: collect elements first,
   then erase them in one pass
2. **Use the erase-remove idiom**: `v.erase(std::remove(...), v.end())`
3. **Always take erase's return value**: `it = container.erase(it)`
4. **Prefer index traversal for vector**: indices are safer than iterators
   across modifications
5. **Know each container's invalidation rules**: they differ per container

## Detection Tools

- **AddressSanitizer**: `-fsanitize=address`
- **Valgrind**: `valgrind --tool=memcheck ./program`

## Example

A runnable version of this topic:

- [pitfall-iterator-invalidation-pitfalls.cpp](../../examples/pitfall-iterator-invalidation-pitfalls.cpp)

## Summary

> **Iterator invalidation is one of the sneakiest sources of bugs in C++.
> Always know the invalidation rules of your container, and re-acquire
> iterators after any modification.**
