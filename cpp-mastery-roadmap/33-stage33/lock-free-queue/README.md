# Lock Free Queue

Wait-free queue using atomic operations.

## Learning Goals

- Build a Michael-Scott queue out of atomics
- Take the payload out of a node only after the exchange that owns it
- Give a popped node back without pulling it from under a reader

## Notes

Two rules keep the queue sound, and both of them are easy to get wrong.

**The payload leaves the node after the exchange.** Two threads can reach
the same node with the same candidate value, and only the one whose
exchange succeeds owns the payload. Moving the value out before the
exchange lets both threads move from the same object at the same time.

**A popped node is retired, not freed.** A thread that is behind may still
be reading the `next` pointer of the node it saw as the head. Freeing that
node right away pulls the ground from under the read, and the descriptor
number may already belong to somebody else by then. The retired nodes are
freed when the queue is destroyed. A production queue would use hazard
pointers or epoch-based reclamation to release them earlier.

## Build

```bash
cmake -S . -B build && cmake --build build
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
