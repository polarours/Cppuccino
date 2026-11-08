# Observer Throttler

An observer with throttled event dispatch.

## Thread Safety

The dispatcher keeps no lock. One thread is meant to emit the events and
one thread is meant to receive them; a dispatcher that is fed from
several threads at once lets more events through than the interval
allows, and setting the handler while a dispatch is going on changes the
`std::function` under the thread that is calling it.

Wrap the dispatcher in a mutex of your own, or use one dispatcher per
thread.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
