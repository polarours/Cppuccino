# Concurrency Pitfall: The Thread-Safety Composition Fallacy

## Problem Description

A common misunderstanding in multithreaded programming is that individually
thread-safe operations compose into a thread-safe whole. They do not: combining
several thread-safe operations can still produce race conditions and data
corruption.

## Wrong Example

```cpp
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <string>

std::map<int, std::string> cache;
std::mutex mtx;

std::string computeExpensiveValue(int key) {
    return "value:" + std::to_string(key);
}

// This function is thread-safe by itself
std::string getValue(int key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second;
    }
    return "";
}

// This one too
void setValue(int key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    cache[key] = value;
}

// ...but composing them is not!
std::string getOrCompute(int key) {
    std::string value = getValue(key);  // lock released here
    if (value.empty()) {
        value = computeExpensiveValue(key);
        setValue(key, value);  // may race with other threads
    }
    return value;
}

int main() { return 0; }
```

## Analysis

The code has the classic "check-then-act" race:

1. **Thread A** calls `getValue(key)`, finds the cache empty
2. **Thread B** also calls `getValue(key)`, also finds it empty (the lock was
   already released)
3. **Threads A and B** both start computing `computeExpensiveValue(key)`
4. **Thread A** calls `setValue` with its result
5. **Thread B** calls `setValue` too, overwriting A's result

## Correct Approaches

### 1. Double-checked locking (correct implementation)

```cpp
#include <mutex>
#include <shared_mutex>
#include <map>
#include <string>

std::map<int, std::string> cache;
std::shared_mutex mtx;

std::string computeExpensiveValue(int key) {
    return "value:" + std::to_string(key);
}

std::string getOrCompute(int key) {
    // First check (no lock held beyond the shared lock)
    {
        std::shared_lock<std::shared_mutex> readLock(mtx);
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
    }

    // Take the write lock
    std::unique_lock<std::shared_mutex> writeLock(mtx);

    // Second check (under the lock)
    {
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
    }

    // Compute and store while holding the write lock
    std::string value = computeExpensiveValue(key);
    cache[key] = value;
    return value;
}
```

### 2. Atomic flag

```cpp
#include <atomic>
#include <mutex>
#include <string>

std::atomic<bool> initialized{false};
std::string value;
std::mutex mtx;

std::string computeExpensiveValue() { return "done"; }

void initOnce() {
    if (initialized.load(std::memory_order_acquire)) {
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);
    if (!initialized.load(std::memory_order_relaxed)) {
        value = computeExpensiveValue();
        std::atomic_thread_fence(std::memory_order_release);
        initialized.store(true, std::memory_order_release);
    }
}
```

### 3. std::call_once

```cpp
#include <mutex>
#include <string>

std::once_flag initFlag;
std::string value;

std::string computeExpensiveValue() { return "done"; }

void initOnce() {
    std::call_once(initFlag, []() {
        value = computeExpensiveValue();
    });
}
```

## Common Trap Patterns

### 1. Check-then-act

```cpp
#include <map>
#include <string>

std::map<int, std::string> cache;

// Danger: the check and the use are not atomic
std::string dangerousGet(int key) {
    if (cache.contains(key)) {
        return cache[key];  // another thread may erase the key in between
    }
    return "";
}
```

### 2. Read-modify-write

```cpp
#include <atomic>

std::atomic<int> counter{0};

// Danger: not an atomic operation
void dangerousIncrement() {
    int value = counter.load();
    counter.store(value + 1);  // another thread may store in between
}
```

### 3. Compound operation

```cpp
void doSomething() {}

// Danger: other threads may interleave between the two operations
void dangerousCompound(bool condition) {
    if (condition) {
        doSomething();
    }
}
```

## Best Practices

1. **Keep lock hold times short**: never run expensive work while holding a lock
2. **Use existing synchronization primitives**: `std::mutex`, `std::atomic`,
   `std::call_once`
3. **Avoid manual lock management**: use RAII wrappers (`std::lock_guard`)
4. **Test concurrent code**: run ThreadSanitizer and friends
5. **Consider lock-free designs**: use concurrent data structures where they fit

## Detection Tools

- **ThreadSanitizer (TSan)**: `-fsanitize=thread` on GCC/Clang
- **AddressSanitizer (ASan)**: memory errors
- **Helgrind**: Valgrind's thread error detector

## Summary

> **Composing individually thread-safe operations does not necessarily yield a
> thread-safe result.**

- Race conditions usually hide in check-then-act sequences
- Protect compound operations with locks or atomic operations
- Prefer the standard library's synchronization primitives
- Use tooling to detect concurrency problems

Remember: **thread safety is not a default property — it is the result of
deliberate design.**
