# SIMD and Performance Optimization

## What Is SIMD?

SIMD (Single Instruction, Multiple Data) is a CPU instruction set — x86
SSE/ARM NEON and their successors such as AVX — where one instruction processes
multiple values at once, e.g. four floats (`__m128`) in a single add. Hand-written
SIMD can dramatically raise throughput in matrix math, signal processing, and
game physics.

> **Core idea**: vectorize scalar work — trade 4× parallel throughput for the
> same amount of data.

## Why Hand-Write SIMD Instead of Trusting the Compiler?

Modern compilers (GCC/Clang at `-O3`) auto-vectorize simple loops, but give up
when the conditions are complex, alignment is poor, or the loop body is
irregular. Hand-written `#pragma omp simd` or intrinsics force the SIMD path
and keep performance from regressing.

## Basics: A Hand-Written Vectorized Add

```cpp
#include <vector>
#include <cstddef>
#include <iostream>

std::vector<float> add(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> result(a.size());

    // vectorized loop: 4 floats per iteration (16-byte aligned)
    std::size_t i = 0;
    for (; i + 4 <= a.size(); i += 4) {
        result[i]     = a[i]     + b[i];
        result[i + 1] = a[i + 1] + b[i + 1];
        result[i + 2] = a[i + 2] + b[i + 2];
        result[i + 3] = a[i + 3] + b[i + 3];
    }
    // scalar tail
    for (; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}
```

The core trick: **vectorized loop + scalar tail**. `i + 4 <= size` keeps the
vector loop in bounds; the scalar loop handles the remainder.

## Dot Product: Parallel Accumulation

The dot product is the classic SIMD-friendly algorithm — each lane accumulates
independently, then the lanes are summed:

```cpp
#include <vector>
#include <cstddef>

float dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
    float sums[4] = {0, 0, 0, 0};   // 4 independent lanes
    std::size_t i = 0;
    for (; i + 4 <= a.size(); i += 4) {
        sums[0] += a[i]     * b[i];
        sums[1] += a[i + 1] * b[i + 1];
        sums[2] += a[i + 2] * b[i + 2];
        sums[3] += a[i + 3] * b[i + 3];
    }
    float total = sums[0] + sums[1] + sums[2] + sums[3];
    for (; i < a.size(); ++i)
        total += a[i] * b[i];
    return total;
}
```

## Lerp (Linear Interpolation): Common on GPUs

```cpp
#include <vector>
#include <cstddef>

std::vector<float> lerp(const std::vector<float>& a,
                        const std::vector<float>& b,
                        float t) {
    float t1 = 1.0f - t;
    std::vector<float> result(a.size());
    std::size_t i = 0;
    for (; i + 4 <= a.size(); i += 4) {
        result[i]     = a[i]     * t1 + b[i]     * t;
        result[i + 1] = a[i + 1] * t1 + b[i + 1] * t;
        result[i + 2] = a[i + 2] * t1 + b[i + 2] * t;
        result[i + 3] = a[i + 3] * t1 + b[i + 3] * t;
    }
    for (; i < a.size(); ++i)
        result[i] = a[i] * t1 + b[i] * t;
    return result;
}
```

## Intrinsics via `<immintrin.h>` / `<arm_neon.h>`

For finer control, use compiler intrinsics (x86 SSE/AVX):

```cpp
#include <cstddef>

#ifdef __x86_64__
#include <immintrin.h>

void add_with_intrinsics(const float* __restrict__ a,
                         const float* __restrict__ b,
                         float* __restrict__ out,
                         std::size_t n) {
    std::size_t i = 0;
    // AVX: 8 floats (256 bits) per iteration
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(out + i, vr);
    }
    // scalar tail
    for (; i < n; ++i)
        out[i] = a[i] + b[i];
}
#endif
```

`_mm256_loadu_ps` / `_mm256_storeu_ps` are the *unaligned* AVX load/store
forms; the aligned versions (`_mm256_load_ps`) are faster but demand 32-byte
alignment.

## Alignment and `alignas`

```cpp
// 32-byte alignment, suitable for aligned AVX instructions
alignas(32) float aligned_a[1024];
alignas(32) float aligned_b[1024];
alignas(32) float aligned_result[1024];
```

Benefits of alignment: the compiler can use `_mm256_load_ps` (about 10–20%
faster than `_mm256_loadu_ps`) and cache misses decrease.

## Best Practices

1. **Get the scalar version right before vectorizing**: correctness first —
   make the scalar path pass tests, then optimize
2. **Vectorization wants contiguous memory**: struct-of-arrays (SoA) suits
   SIMD far better than array-of-structs (AoS)
3. **Don't over-optimize small inputs**: vectorization has fixed overhead;
   below ~1000 elements the scalar version is often faster
4. **Check auto-vectorization with `-fopt-info-vec`**:
   `g++ -O3 -fopt-info-vec-optimized file.cpp` prints the vectorization report
5. **Alignment before intrinsics**: try `alignas` + `-O3` first — the
   compiler vectorizes many simple loops on its own

## Example

A runnable version of this topic:

- [simd-vectorization.cpp](../../examples/simd-vectorization.cpp)

## Summary

The core pattern of hand-written SIMD is **"vectorized loop + scalar tail"**,
with `i + 4 <= size` keeping the stride inside bounds. Aligned memory via
`alignas` is step one; intrinsics are a precision tool, not the daily default —
for most cases `-O3` is enough.
