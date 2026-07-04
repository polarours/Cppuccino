# Lvalue References

## What is an Lvalue Reference?

An lvalue reference (`T&`) is a reference that binds to objects with identity — objects that have a name, occupy a identifiable location in memory, and persist beyond a single expression.

```cpp
int x = 42;
int& ref = x;    // OK: x is an lvalue
ref = 100;       // Modifies x directly

// int& bad = 42;  // ERROR: 42 is an rvalue
```

## Key Properties

### 1. Binding Rules

Lvalue references bind to lvalues only (with one exception for `const` lvalue references):

```cpp
int x = 10;
int& r1 = x;       // OK: lvalue to lvalue reference
// int& r2 = 42;   // ERROR: cannot bind non-const lvalue reference to rvalue

const int& r3 = 42;  // OK: const lvalue reference can bind to rvalue
```

### 2. Cannot Be Rebound

Once initialized, an lvalue reference always refers to the same object:

```cpp
int x = 10;
int y = 20;
int& ref = x;
ref = y;          // This modifies x, not rebinds ref to y
// ref is still referring to x, which now has value 20
```

### 3. Must Be Initialized

Lvalue references must be initialized when declared:

```cpp
int x = 10;
int& ref = x;     // OK
// int& bad;       // ERROR: uninitialized reference
```

## Common Use Cases

### Function Parameters

Lvalue references allow functions to modify their arguments:

```cpp
void increment(int& value) {
    ++value;
}

int main() {
    int x = 5;
    increment(x);  // x is now 6
}
```

### Avoiding Copies

Pass large objects by reference to avoid expensive copies:

```cpp
void process(const std::string& str) {
    // Read-only access without copying
    std::cout << str << '\n';
}
```

### Returning Multiple Values

Use references to "return" multiple values:

```cpp
void divide(int dividend, int divisor, int& quotient, int& remainder) {
    quotient = dividend / divisor;
    remainder = dividend % divisor;
}
```

## Pitfalls

### Dangling References

A reference becomes dangling when the object it refers to is destroyed:

```cpp
int& getRef() {
    int local = 42;
    return local;  // BAD: returning reference to local
}

int& ref = getRef();  // Dangling reference!
```

### Accidental Copies in Templates

Forwarding references are different from lvalue references:

```cpp
template <typename T>
void process(T& arg) {
    // Only accepts lvalues
}

template <typename T>
void process(T&& arg) {
    // Accepts both lvalues and rvalues (forwarding reference)
}
```

## Lvalue Reference vs Pointer

| Feature | Lvalue Reference | Pointer |
|---------|------------------|---------|
| Initialization | Required | Optional |
| Rebinding | Not allowed | Allowed |
| Nullability | Cannot be null | Can be null |
| Syntax | Direct access | Dereference needed |
| Safety | Safer | More flexible |

## Summary

- Lvalue references bind to named objects
- They cannot be rebound after initialization
- Use `const T&` for read-only access and to accept rvalues
- Avoid returning references to local objects
- Lvalue references are safer than pointers but less flexible
