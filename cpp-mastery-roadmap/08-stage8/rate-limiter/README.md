# Rate Limiter

Three classic algorithms behind one header: token bucket (burst +
sustained rate), fixed window counter (per-key), and sliding window log.

## Learning Goals

- Token bucket: burst capacity vs refill rate, capping at capacity
- Fixed window: cheap but allows boundary bursts (2x limit)
- Sliding window log: exact but stores per-request timestamps
- Injecting `now` as a parameter makes timing logic deterministic in tests

## Non-Goals

- Distributed rate limiting (Redis / Lua scripts)
- Weighted requests (cost-based admission)
- Header emission (X-RateLimit-*)

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
