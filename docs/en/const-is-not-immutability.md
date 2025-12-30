# const is not "immutable", but an interface contract

In C++, `const` is often misunderstood as "object cannot be modified".
This understanding is not wrong, but very incomplete. More accurately, `const` is an interface-level commitment (interface contract):

> In one sentence: I guarantee to the caller: the observable state of the object through this interface will not change.

## What is const?

### Logical State vs Physical State

Before discussing `const`, we must distinguish between two levels of "state":

- **Logical state**
  - Refers to the behavior and attributes that the object exhibits externally. For example, the logical state of a counter object is its current count value.
- **Physical state**
  - Refers to the actual data storage and implementation details inside the object. For example, a counter object may have some cache variables or auxiliary data structures that are part of its physical state.

`const` constrains the logical state, not whether physical memory is written to.

Let's consider the following example:

```cpp
class Counter {
public:
    void increment() {
        ++count_;
        cache_valid_ = false;
    }

    int getCount() const {
        if (!cache_valid_) {
            cached_value_ = count_; // Modify physical state (for caching only), logical state count_ unchanged
            cache_valid_ = true;
        }
        return cached_value_;
    }

private:
    int count_ = 0;

    // The following members do not belong to logical state
    // mutable does not break const, but is used to explicitly mark: this member does not belong to the object's logical state.
    mutable bool cache_valid_ = false;
    mutable int cached_value_ = 0;
};

int main() {
    Counter c;
    c.increment();
    int value = c.getCount(); // Accessed through const method
    return 0;
}
```

In this example, the `getCount` method is declared as `const`, meaning it does not modify the logical state of the `Counter` object (i.e., `count_`). However, it does modify the member variables `cache_valid_` and `cached_value_`, which are part of the physical state.

> Note: The division between logical and physical state is not language-enforced, but rather a design concept. In actual projects, reasonable division helps performance optimization and interface design.

### The True Meaning of const

When we use `const` to modify an object, we are telling the compiler and reader: "Through this interface, I promise not to modify the logical state of this object." This does not mean that the object's physical state is completely immutable. For example, some member variables might be marked as `mutable`, allowing them to be modified in `const` methods.

Let's look at the following example:

```cpp
class Example {
public:
    int getValue() const {
        accessCount++; // Allowed to modify, because accessCount is mutable
        return value;
    }
private:
    int value = 42;
    mutable int accessCount = 0; // This member can be modified in const methods
};
```

In this example, `accessCount` is declared as `mutable`, so even though in the `getValue` method (which is `const`), we can still modify `accessCount`. This shows that `const` does not mean all members of an object are immutable, but emphasizes the invariance of the logical state.

## In-depth Analysis of const Design Philosophy

### Interface Contract

In C++, "interface contract" is a design concept that emphasizes clearly specifying the behavior norms of objects or functions to users through function signatures, type modifiers, class interfaces, etc. Simply put, it's "what I promise to do and not do through this interface".
In the context of `const`, this contract means: when accessing an object through a `const` interface, the caller can rely on the fact that this interface will not change the semantic state (logical state) of the object.

> Of course, interface contracts are not limited to `const`; they also include other aspects, such as exception safety guarantees, thread safety guarantees, etc.

### Performance Optimization

Using `const` can also help the compiler perform optimizations. Because the compiler knows that certain objects will not be modified, it can be more aggressive with optimizations, such as caching results, eliminating unnecessary copies, etc.

For example, we can write a simple test program to compare the performance difference between const and non-const methods. The specific demonstration code can be found in [const_vs_nonconst.cpp](/examples/const_vs_nonconst.cpp).

### Code Readability and Maintainability

Using `const` can improve code readability and maintainability. By clearly marking which objects are not modifiable, developers can more easily understand the intent of the code and reduce misuse and errors.

For example:

```cpp
void printValue(const int& value) {
    std::cout << value << std::endl;
}
```

In this example, `const int& value` clearly indicates that the `printValue` function will not modify the passed parameter, which helps the caller understand the function's behavior.

## Summary

- `const` is not "object immutable", but an interface contract that promises not to modify the object's logical state.
- Distinguishing between logical state and physical state helps understand the true meaning of `const`.
- Using `const` can improve code performance, readability, and maintainability.