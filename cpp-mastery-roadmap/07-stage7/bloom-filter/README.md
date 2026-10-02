# Bloom Filter

Probabilistic membership structure: no false negatives, tunable false
positives, k hash functions via double hashing over a packed bit array.

## Learning Goals

- Why bloom filters never false-negative (bits are only set, never cleared)
- Double hashing: derive k positions from 2 base hashes
- False-positive rate formula and how m/k/n trade off
- Packed bit storage (uint8_t array + shift/mask)

## Non-Goals

- Deletion (Counting Bloom Filter)
- Scalable / partitioned variants
- Cryptographic hash requirements

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
