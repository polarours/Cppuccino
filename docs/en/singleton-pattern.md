# Singleton Pattern

## What Is the Singleton Pattern?

The singleton pattern guarantees that a class has **exactly one instance** and
provides a global access point to it.

> **Core idea**: make the constructor private and return the same object from
> `getInstance()`.

## When It Fits

- Loggers — one per process
- Configuration centers — one consistent view per process
- Thread pools, connection pools, schedulers

## Core Implementation (Meyers Singleton)

Since C++11 the recommended form is the **Meyers Singleton**: a function-local
`static` is constructed on first call, and thread safety is guaranteed by the
C++ runtime ("Magic Statics").

```cpp
#include <string>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;   // thread-safe since C++11
        return instance;
    }
    void log(const std::string& msg) { /* ... */ }
private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
```

## Comparing Three Implementations

| Implementation | Thread-safe | Lazy construction | Recommendation |
|---|---|---|---|
| Function-local `static` (Meyers) | ✅ since C++11 | ✅ | ★★★★★ |
| `std::call_once` + `std::once_flag` | ✅ | ✅ | ★★★ |
| Eager (pointer + manual `init`) | implementation-defined | ❌ | ★ |

**Do not** hand-roll double-checked locking (DCLP): before C++11 it required
correct `volatile` + memory barriers and is extremely easy to get wrong; after
C++11, Meyers is the zero-cost best answer.

## Implementation

```cpp
#include <iostream>
#include <map>
#include <mutex>
#include <string>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[LOG] " << message << "\n";
        ++count_;
    }
    int getLogCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    int count_ = 0;
    mutable std::mutex mutex_;
};

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }
    void set(const std::string& k, const std::string& v) { config_[k] = v; }
    std::string get(const std::string& k, const std::string& def = "") const {
        auto it = config_.find(k);
        return it != config_.end() ? it->second : def;
    }
private:
    Config() = default;
    std::map<std::string, std::string> config_;
};
```

For the full runnable example, see the
[singleton-pattern project](../../cpp-mastery-roadmap/13-stage13/singleton-pattern).

## Pitfalls and Best Practices

1. **Delete the copy constructor and assignment**: otherwise
   `Logger a = Logger::getInstance();` quietly breaks uniqueness
2. **Never touch other singletons in a destructor**: destruction order across
   singletons is unspecified (`std::exit` runs `atexit` handlers in an
   implementation-defined order) — cross-singleton access can dangle
3. **Do not use a singleton as a global variable**: it is a *controlled* global,
   only slightly better than `int g_count = 0;` — prefer explicit dependency
   injection
4. **Testability cost**: tests need to "reset the singleton" or "inject a
   double" — the Meyers form makes resets awkward; the usual answers are a
   `clear()` method or switching to an interface in tests
5. **In-process vs cross-process**: a singleton is unique only within **one**
   process; multi-process cases need extra machinery (shared memory / file
   locks)

## When Not to Use a Singleton

- The object legitimately has multiple instances (several connections in a
  connection pool)
- It is only ever passed around — put it in the constructor parameters instead
- Tests need to replace it — prefer an interface + factory

## Summary

The Meyers singleton is the cleanest and safest singleton in modern C++; but a
singleton is still a "controlled global" — reserve it for cases that genuinely
need process-wide uniqueness and have no reasonable alternative.
