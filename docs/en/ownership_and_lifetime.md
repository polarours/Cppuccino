# Ownership and Lifetime

## 1. What is a Lifetime?

In C++, an object is not considered "alive" simply because it exists in memory. A lifetime describes the period during which an object is formally recognized by language rules as a "valid object."

More precisely:

> The lifetime of an object begins when its construction is complete and ends when its destruction begins.

This definition may seem simple, yet it explains nearly all issues related to resources, safety, and performance in C++.

### Lifetime is not Scope, but Often Determined by Scope

Scope describes the range in which names (variable names, function names, etc.) are visible in the code, while lifetime describes the period during which an object is "alive" in memory. The two are related but not identical.

Let's first look at a simple example:

```cpp
void foo() {
    int x = 42; // x's lifetime begins here
    // use x
} // x's lifetime ends here
```

In this example, variable `x`'s lifetime is the same as its scope: from declaration to function end.

This is the most common case in C++, but not the only case.

```cpp
int* createInteger() {
    int localVar = 1024; // localVar's lifetime is limited to this function
    return &localVar;    // Error! Returning the address of a local variable
}
```

In this example, `localVar`'s lifetime ends when the function returns. Although its memory address may still exist, the object itself is no longer valid. Returning that address directly leads to a **dangling pointer** problem.

### Construction and Destruction Define the Boundaries of Object Existence

We say lifetime is the period during which an object is "alive" in memory, but lifetime doesn't care about "whether the memory still existed before" — it only cares about two things:
- When the constructor is called
- When the destructor is called

```cpp
class MyClass {
public:
    MyClass() { /* construction logic */ }
    ~MyClass() { /* destruction logic */ }
};

void example() {
    MyClass obj; // obj's lifetime begins here
    // use obj
} // obj's lifetime ends here, destructor is called
```

In this example, `obj`'s lifetime begins when it's created and ends when it leaves scope, with the destructor being called at the end of the lifetime.

After the lifetime ends:
- Memory may still be retained, but the object is no longer legal
- The pointer value may still point to the original address
- But the object is no longer a valid object instance

This is one of the most important rules in C++:

> Any access to an object after its lifetime has ended is undefined behavior.

## 2. Who Determines Lifetime?

In C++, object lifetimes never occur automatically; they are always determined by some explicit mechanism or code.

In other words:

> Every object has an answer to "who keeps it alive, and who ends it."

### Lifetime is Not Determined by the Object Itself

An object doesn't "born itself" or "die itself." The beginning and ending of lifetime are always controlled by the code that created it.

```cpp
void example() {
    int x = 1024;
}
```

In this example:
- `x`'s lifetime begins when execution reaches the declaration statement
- `x`'s lifetime ends when leaving scope

Determining all of this is not `x` itself, but the scope rules.

### Different Storage Types Have Different Lifetime Rules

- **Stack objects**: Their lifetimes are typically determined by scope and are destroyed when leaving scope.
- **Heap objects**: Their lifetimes are controlled by `new` and `delete`, requiring explicit management.
- **Static objects**: Their lifetimes span from program start to program end, usually managed by the compiler.

#### Stack Objects: Lifetime Determined by Scope

```cpp
void foo() {
    MyClass obj;
    // obj is a valid object here
}
// Leaving scope, obj's destructor is called, lifetime ends
```

In this case:
- Lifetime is deterministic
- Destruction timing is predictable
- No explicit management needed

This is why `RAII` naturally fits stack objects.

> Strictly speaking, the term "stack object" does not exist in the C++ standard. A more accurate term is **Automatic Storage Duration object**, whose lifetime is automatically managed by entering and exiting scopes. Most implementations allocate them on the call stack, so in engineering practice, they are habitually called "stack objects."

#### Heap Objects: Lifetime Determined by Owner

Once an object is allocated on the heap, the situation changes.

```cpp
MyClass* ptr = new MyClass();
// ...
delete ptr;
// Using smart pointers (such as std::unique_ptr or std::shared_ptr) can automatically manage dynamic object lifetimes, avoiding the problems we will discuss next—memory leaks and double frees.
```

Here:
- Object lifetime begins after `new` completes construction
- Object lifetime ends before `delete` destruction
- Requires explicit lifetime management

If `delete` never occurs, the object remains "alive" forever — that is, a memory leak.
If `delete` occurs multiple times, it leads to undefined behavior.

> For heap objects, lifetime is no longer implicitly determined by scope, but by "who is responsible for freeing it."

#### Static Objects: Lifetime Spans the Entire Program

Static objects (Static Storage Duration objects) include global objects and static local objects within functions.

A very simple example:

```cpp
static MyClass staticObj; // This is a static object
```

Here:
- `staticObj`'s lifetime constructs at program start and destructs at program end
- Lifetime is managed by the compiler and linker
- No explicit management needed

## 3. Deep Understanding of Lifetime

In our previous discussions, we've learned that lifetime spans from construction to destruction, and that objects with different storage durations have different lifetime rules. But if we stop here, lifetime remains just a "language mechanism." The truly important question is:

> Why does lifetime exist in this way? And why is it almost always bound to resource safety?

The answer is only one: **Ownership**.

### Lifetime is Not an Isolated Concept

In C++, lifetime is never a "naturally passing" period of time. It can be understood as:

> An object's lifetime is the period during which it is legitimately managed by some entity (owner) — whoever is responsible for construction and destruction determines when the object becomes effective and is destroyed. Lifetime and ownership are not completely equivalent, but in C++, they are highly correlated.

Here:
- When an object is created, it must have an "owner" — the entity responsible for managing its lifetime.
- When this owner decides the object is no longer needed, it ends the object's lifetime.

When we re-examine the previous three categories of objects from this perspective, everything becomes clear.

- Stack objects: Implicit ownership, lifetime determined by scope
- Heap objects: Explicit ownership, lifetime determined by `new` and `delete`
- Static objects: Global ownership, lifetime spans the entire program

## 4. Ownership Can Lead to Lifetime Errors

In actual engineering, the vast majority of lifetime errors are not "syntax errors," but:

> The user of an object is not equal to the owner of the object.

### Dangling Pointer

A dangling pointer occurs precisely because the user of an object still attempts to access the object after its lifetime has ended.

```cpp
int* createInteger() {
    int localVar = 1024; // localVar's lifetime is limited to this function
    return &localVar;    // Error! Returning the address of a local variable
}
```

The root of the problem is not "returning a local variable address," but:
- The caller of `createInteger` function attempts to use an object it doesn't own.
- `localVar`'s owner is the `createInteger` function. Once the function returns, `localVar`'s lifetime ends, and the caller accessing it again is undefined behavior.

When ownership ends while the user still exists, dangling pointers are inevitable.

### Double Free

Double free occurs precisely because object ownership is held by multiple entities simultaneously, causing the same object to be freed multiple times.

```cpp
int* ptr = new int(42);
delete ptr; // First free
delete ptr; // Error! Second free, undefined behavior
// If multiple pointers point to the same object, ensure only one pointer is responsible for freeing
```

The root of the problem lies in:
- `ptr`'s ownership has no clear attribution, causing the same object to be freed by multiple entities.
- After the first `delete`, `ptr` no longer owns the object's ownership. The second `delete` operates on an object whose lifetime has already ended.

When ownership is unclear and multiple entities attempt to manage the same object, double frees are inevitable.

### Memory Leak

A memory leak occurs precisely because no entity assumes object ownership, causing the object's lifetime to never end.

```cpp
int* ptr = new int(42);
// Forgot to delete ptr, causing a memory leak
```

Why must we `delete`?

Because:
- `ptr`'s ownership is given to the caller at allocation time.
- If the caller doesn't free it, the object's lifetime will never end, and the system cannot reclaim this memory.

When ownership is unclear and the object is not explicitly freed, memory leaks are inevitable.

### Common Modern Solutions

The problems we mentioned above, including dangling pointers, double frees, and memory leaks, are all caused by improper ownership management. Modern C++ provides various tools to help us better manage ownership, thereby avoiding lifetime errors.

We can use:
- `std::unique_ptr`: Implements exclusive ownership, preventing double frees
- `std::shared_ptr`: Implements shared ownership, automatically managing reference counting
- `std::weak_ptr`: Implements non-owning observation, preventing dangling pointers
- `RAII` design pattern: Ensures resources are properly managed during object lifetime

> For more specific code examples and usage methods, please refer to subsequent sections (to be added).

## 5. Summary

- Lifetime describes the period during which an object exists validly from construction to destruction.
- Lifetime is determined by ownership; whoever owns the object is responsible for managing its lifetime.
- Improper ownership management often leads to dangling pointers, double frees, and memory leaks.
- Understanding the relationship between ownership and lifetime is the foundation for writing safe and efficient C++ code.
