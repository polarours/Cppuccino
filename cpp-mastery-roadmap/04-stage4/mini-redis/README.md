# Mini Redis

In-memory KV store with a Redis-like command set (SET/GET/DEL/INCR/EXPIRE/
TTL/DBSIZE), RESP-style replies, and lazy TTL expiry.

## Learning Goals

- Command dispatch table with arity checks and uniform error replies
- Lazy expiration: why access-time checks are enough for correctness
- TTL semantics: -2/-1/rounding, expiry on write paths
- Integer-parsing discipline for INCR (reject trailing garbage)

## Non-Goals

- RESP multibulk parsing and quoted strings
- Real TCP listener (wire the Store into a server from stage 4)
- Persistence / replication

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
