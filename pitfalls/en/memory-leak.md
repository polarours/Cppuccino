# Memory Leaks

## Introduction

A *memory leak* happens when a program allocates memory but loses the ability to free it. In C++, leaks are almost always the symptom of one root cause: **manually managing ownership with raw pointers instead of letting the type system do it for you.**

Modern C++ gives you tools (`std::unique_ptr`, `std::shared_ptr`, containers) that make leaks nearly impossible *by construction*. Yet leaks remain one of the most common bugs, especially in codebases that still use `new`/`delete` by hand.

## The Problem

```cpp
#include <string>

struct Record {
    std::string name;
    int value;
};

void process(bool skip) {
    Record* r = new Record{"alpha", 1};   // (1) ownership begins here

    if (skip) {
        return;                            // (2) LEAK: r is never deleted
    }

    // ... more work that might throw ...

    delete r;                             // (3) only reached on one path
}
```

**Why it's broken:**
- If `skip` is `true`, the function returns at (2) and `r` is never freed — a leak on every such call.
- If any code between (1) and (3) throws an exception, control leaves the function without reaching `delete` — a leak on the error path.
- The leak is silent: the program keeps running, slowly eating RAM until it is killed by the OOM killer or crawls to a halt. `new` does not raise an exception for "you forgot to free"; it only raises `std::bad_alloc` when the *whole system* runs out.

Even the "happy path" is fragile: every added early-return or thrown exception becomes a new leak site.

## The Fix

Use `std::unique_ptr` so destruction is tied to scope, not to a manually written `delete`:

```cpp
#include <memory>
#include <string>

struct Record {
    std::string name;
    int value;
};

void process(bool skip) {
    auto r = std::make_unique<Record>(Record{"alpha", 1});  // owns the object

    if (skip) {
        return;                            // OK: r is destroyed here, memory freed
    }

    // ... if this throws, the stack unwinds and r is destroyed automatically ...

}   // r goes out of scope here regardless of how we leave -> no leak, ever
```

If ownership needs to be shared or moved across functions, use `std::shared_ptr` or move a `std::unique_ptr`:

```cpp
std::unique_ptr<Record> load() {
    return std::make_unique<Record>(Record{"beta", 2});   // ownership transferred out
}

void use(std::unique_ptr<Record> r) { /* owns it; freed on return */ }
```

When the resource is *not* memory (file handle, socket, lock), write a small RAII wrapper or use an existing one (`std::lock_guard`, `std::fstream`): the same "destructor frees it" rule applies.

## Best Practices

- **Never write a naked `new`/`delete` in application code.** If you feel you need one, you almost always want `std::make_unique` / `std::make_shared` instead.
- **One owner per resource.** `std::unique_ptr` encodes single ownership; `std::shared_ptr` encodes shared ownership with a clear reference count. Raw pointers should only ever *observe*, never own.
- **Let containers own their elements.** Store `std::vector<Record>` rather than `std::vector<Record*>`; the vector's destructor frees everything.
- **Catch leaks early with tooling.** Run debug builds under AddressSanitizer (`-fsanitize=address`) or Valgrind; both report exactly where the leaked block was allocated.
- **Treat "trivial" early returns as leak hazards.** Any `return` / `break` / `continue` / exception past a `new` is a potential leak unless the object is RAII-owned.

## Summary

Memory leaks in C++ are rarely subtle logic errors — they are ownership errors. The fix is not "remember to call `delete` more carefully"; it is to stop managing memory by hand. Wrap every resource in an RAII type (`std::unique_ptr`, `std::shared_ptr`, a container, or a custom guard) so that leaving a scope — by any path, normal or exceptional — frees the resource automatically. Leaks then become a compile-time non-issue rather than a runtime hunt.
