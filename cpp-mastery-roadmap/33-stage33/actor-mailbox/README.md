# Actor Mailbox

Bounded mailbox with a single actor thread: handlers run without holding
the lock, producers never block the actor.

## Learning Goals

- Why "one consumer thread" removes locks inside message handling
- Two-phase wait: drop the lock before running the handler
- Bounded mailboxes as backpressure (send returns false when full)
- Graceful shutdown: drain-then-exit vs discard semantics

## Non-Goals

- Actor supervision trees / restarts
- Distributed actors
- Prioritized mailboxes

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
