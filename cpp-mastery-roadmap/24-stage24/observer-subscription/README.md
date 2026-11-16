# Observer Subscription

Observer with RAII subscription management.

A handler may unsubscribe itself while the event is being delivered: the
handlers are copied before the delivery starts, so that changing the
subscription list does not disturb the delivery in progress.

The subscription must not outlive the bus: it holds a way back to it, and
that way back is gone once the bus is.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
