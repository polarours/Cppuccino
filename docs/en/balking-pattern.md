# Balking Pattern

## What Is the Balking Pattern?

The balking pattern lets an object **return immediately** when a precondition
is not met — no blocking, no exception, no retry. The caller treats it as
"nothing happened".

> **Core idea**: a guard check decides whether to proceed; if the condition
> fails, exit right away.

## How It Relates to Guarded Suspension

| | Balking | Guarded Suspension |
|---|---|---|
| Condition not met | Return immediately | Wait until the condition holds |
| Caller receives | "No operation" | A successful result |
| Fit | One-shot chances, droppable events | Requests that must complete |
| Complexity | Low (no condition variable) | Medium (condition variable + lock) |

## Core Concepts

- **Guard**: a check of the state field at method entry
- **Balks**: `return` when the guard fails (no throw, no retry)
- **Idempotency**: repeated calls are safe because repeating a balk has no
  side effects

## When It Fits

- The device is already open — a second `open()` is a no-op
- Skipping processing when data is not ready (dropping frames is acceptable in
  streaming)
- The user is already logged in — clicking login again does nothing
- Single-shot events (a second press shortly after the first is ignored)

## Problem Scenario

```cpp
// Problem: expressing "already open" with an exception
struct AlreadyOpen {};

class Document {
public:
    void open() {
        if (opened_) throw AlreadyOpen{};
        opened_ = true;
    }
private:
    bool opened_ = false;
};
// callers must try-catch: poor readability and needless cost
```

### Solution

```cpp
#include <mutex>

class Document {
public:
    void open() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (opened_) return;     // silent balking
        opened_ = true;
    }
    void save() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;    // don't save what was never opened
        saved_ = true;
    }
private:
    std::mutex mutex_;
    bool opened_ = false;
    bool saved_ = false;
};
```

## Implementation

```cpp
#include <atomic>
#include <mutex>

class Document {
public:
    void open() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (opened_) return;
        opened_ = true;
    }
    void save() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;
        saved_ = true;
    }
    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!opened_) return;
        opened_ = false;
        saved_ = false;
    }
    bool isOpen() const { return opened_; }
    bool isSaved() const { return saved_; }

private:
    std::mutex mutex_;
    bool opened_ = false;
    bool saved_ = false;
};

class Sensor {
public:
    explicit Sensor(int threshold = 100) : threshold_(threshold) {}

    // lock-free version: single producer, single consumer
    bool read(int value) {
        if (value > threshold_) {
            lastValue_ = value;
            triggered_ = true;
            return true;
        }
        return false;             // below threshold: balk
    }
    bool wasTriggered() const { return triggered_; }
    int lastValue() const { return lastValue_; }

private:
    int threshold_;
    std::atomic<bool> triggered_{false};
    std::atomic<int> lastValue_{0};
};
```

For the full runnable example, see the
[balking project](../../cpp-mastery-roadmap/31-stage31/balking).

## Best Practices

1. **Document the balking semantics**: callers must know a call may be a
   no-op — the method name should hint at it (e.g. `tryOpen`)
2. **Guard check and state mutation inside the same lock**: avoids TOCTOU
   races
3. **Never treat a balk as silent failure**: on critical paths, log the balk
   once for later debugging
4. **Idempotency is a precondition for balking**: a balked method must not
   leave dirty state behind when called repeatedly

## Summary

Balking replaces "block or throw" with "nothing happened" — a tidy tool for
idempotent operations and short debounce windows; used well it makes APIs more
tolerant, as long as callers know explicitly that this time nothing occurred.
