# Observer Pattern

## What Is the Observer Pattern?

The observer pattern defines a **one-to-many dependency** between objects: when
the "subject's" state changes, every "observer" is notified.

> **Core idea**: the subject never calls observers directly — it broadcasts
> through an interface, and observers subscribe/unsubscribe themselves.

## When It Fits

- Event buses (UI updates, message dispatch)
- Real-time data push (market data, heartbeats)
- Model-view synchronization (a View subscribing to a Model in MVC)
- One state change triggering several independent handlers (cache invalidation
  + logging + alerting)

## Core Concepts

- **Subject**: keeps the observer list and provides
  `attach / detach / notify`
- **Observer**: implements the `update(value)` interface
- **Weak vs strong references**: store observers as `weak_ptr` to avoid
  reference cycles; the Subject must not extend observers' lifetimes
- **Push vs pull model**: push carries the data in `update(value)`; pull only
  announces the change and the observer actively calls `subject.getValue()`

## Observer vs Pub/Sub

| | Observer | Pub/Sub |
|---|---|---|
| Intermediary | None (direct calls) | Event bus / message middleware |
| Coupling | Subject knows the Observer type | Publisher and subscriber don't know each other |
| Sync style | Usually synchronous | Often asynchronous |
| Scope | In-process | Cross-process / cross-network |

## Implementation

```cpp
#include <algorithm>
#include <memory>
#include <vector>

template <typename T>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const T& value) = 0;
};

template <typename T>
class Subject {
public:
    void attach(std::shared_ptr<Observer<T>> observer) {
        observers_.push_back(std::move(observer));
    }

    void detach(std::shared_ptr<Observer<T>> observer) {
        observers_.erase(
            std::remove_if(observers_.begin(), observers_.end(),
                [&observer](const std::weak_ptr<Observer<T>>& wp) {
                    return wp.expired() || wp.lock() == observer;
                }),
            observers_.end());
    }

    void notify(const T& value) {
        // copy the list: callbacks may modify the original
        auto snapshot = observers_;
        for (auto& w : snapshot) {
            if (auto sp = w.lock()) sp->update(value);
        }
    }

    void setValue(T value) {
        value_ = value;
        notify(value_);
    }
    T getValue() const { return value_; }

private:
    T value_{};
    std::vector<std::weak_ptr<Observer<T>>> observers_;
};

template <typename T>
class Logger : public Observer<T> {
public:
    void update(const T& v) override { /* std::cout << v << '\n'; */ }
private:
    int updates_ = 0;
};
```

For the full runnable example, see the
[observer-pattern project](../../cpp-mastery-roadmap/09-stage9/observer-pattern).

## Usage Example

```cpp
Subject<int> temperature;
auto display = std::make_shared<Logger<int>>();
auto alarm   = std::make_shared<Logger<int>>();

temperature.attach(display);
temperature.attach(alarm);

temperature.setValue(42);    // triggers display.update + alarm.update
temperature.detach(alarm);
temperature.setValue(43);    // only display left
```

## Key Traps

1. **The observer list is modified during notification**: `attach/detach`
   during iteration invalidates iterators. Always `snapshot` a copy first
2. **Observers holding a strong reference to the Subject** → reference cycle.
   Solve with `weak_ptr`
3. **Exception propagation**: if one observer `throw`s, the rest are never
   notified. Wrap each `update` in `try/catch`
4. **Lifetime mismatch**: touching the Subject after it was destroyed dangles —
   skip when `weak_ptr.lock()` fails
5. **Ordering**: the call order among observers must be treated as undefined;
   they must not depend on each other logically

## When Not to Use It

- A change triggers exactly one handler → call it directly
- Cross-process, cross-language → a real message queue (ZeroMQ / Kafka)
- One-to-one notification → function pointer / `std::function`

## Example

A runnable version of this topic:

- [observer-pattern-demo.cpp](../../examples/observer-pattern-demo.cpp)

## Summary

Observer is the smallest unit of event-driven architecture; a correct
implementation hinges on "list copy + weak references + exception isolation" —
miss any one of the three and production stability will show you why.
