# Timer Wheel

Single-level timing wheel: O(1) schedule/cancel by absolute tick, with
re-check on rotation so far-future timers survive a full lap.

## Learning Goals

- Slot index = absolute tick modulo wheel size
- Why storing absolute fire times avoids double-fire on rotation
- Cancel by id scanning one wheel
- Contrast with a binary-heap timer queue (O(log n), no slot granularity)

## Non-Goals

- Hierarchical wheels (min/max timing wheel)
- Thread safety (wrap with a mutex for production)
- High-resolution (millisecond) real-time guarantees

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
