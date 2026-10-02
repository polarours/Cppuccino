# LFU Cache

Least Frequently Used cache with O(1) get/put, frequency buckets with
LRU tie-breaking.

## Learning Goals

- Frequency-bucket design: map<freq, LRU list> + node iterator index
- Why frequency buckets keep operations O(1)
- Tie-breaking policy when frequencies are equal
- Compare against LRU (see ../lru-cache)

## Non-Goals

- TinyLFU / window admission
- Thread safety
- TTL-based expiration

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
