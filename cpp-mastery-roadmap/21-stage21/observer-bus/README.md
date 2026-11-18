# Observer Bus

An event bus for publish-subscribe communication.

Dispatch iterates a snapshot of the subscriber list, so a handler may
subscribe to the same event while it is being dispatched; the new handler
starts with the next publish.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
