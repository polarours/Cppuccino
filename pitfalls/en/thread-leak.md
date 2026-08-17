# Thread Leak

## 1. Introduction

A **thread leak** happens when threads are created but never properly reclaimed.
This takes two damaging forms: the `std::thread` object is destroyed while still
joinable (which calls `std::terminate` and aborts the program), or threads are
detached and left running, silently consuming resources until process exit.

Thread leaks waste memory, stack space, and file descriptors, and can eventually
exhaust the system's thread limit.

## 2. The Problem

### 2.1 Destroying a joinable thread

```cpp
#include <thread>

void task() { /* ... */ }

void spawn() {
    std::thread t(task);
    // oops: t goes out of scope without join() or detach()
}  // std::terminate() is called here — the program aborts
```

`std::thread`'s destructor calls `std::terminate()` if the thread is still
joinable. This is a hard crash, not a quiet leak.

### 2.2 Detaching and forgetting

```cpp
void spawn_detached() {
    std::thread t([] {
        do_long_work();  // runs "in the background"
    });
    t.detach();  // no way to join or monitor it later
}
```

After `detach()`, the thread keeps running independently. If it captures
pointers or references to local variables that have since gone out of scope, it
becomes a use-after-free. Even when memory-safe, detached threads accumulate and
are difficult to shut down cleanly.

## 3. The Fix

### 3.1 Always join via an RAII wrapper

Wrap the thread so it is joined automatically at scope exit:

```cpp
#include <thread>

class joining_thread {
    std::thread t_;
public:
    explicit joining_thread(std::thread t) : t_(std::move(t)) {}
    ~joining_thread() { if (t_.joinable()) t_.join(); }
    // operator=, and other forwarding omitted for brevity
};

void spawn() {
    joining_thread t{std::thread(task)};  // joined on scope exit
}
```

### 3.2 Use `std::jthread` (C++20)

`std::jthread` joins automatically in its destructor — no manual RAII needed,
and it also supports cooperative cancellation via a stop token:

```cpp
#include <thread>

void spawn() {
    std::jthread t(task);  // automatically joined at end of scope
}
```

### 3.3 Manage lifetimes explicitly

If you truly need long-lived threads, store them in a container, give them a
shutdown flag, and join them during teardown.

## 4. Best Practices

1. **Never let a `std::thread` go out of scope while joinable.**
2. **Prefer `std::jthread`** (C++20) or a joining RAII wrapper.
3. **Avoid `detach()`** unless you have a lifetime story for captured data.
4. **Track threads in a registry** so you can join them on shutdown.
5. **Use a stop token / flag** for cooperative cancellation.

## 5. Summary

- A joinable `std::thread` destroyed = `std::terminate()` (crash).
- `detach()` leaks control and risks use-after-free on captured locals.
- Fix with `std::jthread` or a joining RAII wrapper.
- Always have a clean shutdown path that joins your threads.
