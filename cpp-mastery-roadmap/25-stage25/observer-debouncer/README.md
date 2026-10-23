# Observer Debouncer

Observer that delivers an event to its handler after a delay.

## Learning Goals

- Deliver an event from another thread
- Stop a pending delivery before the dispatcher is gone
- Keep the handler and the dispatcher's state consistent

## Features

- `emit()` schedules a delivery; the handler runs on its own thread
- `cancel()` stops the deliveries that have not happened yet
- The destructor cancels and waits, so no handler runs after the dispatcher
  is gone

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```

## Note

Every `emit()` is delivered on its own; the delay is how long the
dispatcher waits before it calls the handler. The class does not merge
several events into one.
