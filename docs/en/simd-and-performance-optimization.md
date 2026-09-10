# SIMD 与性能优化 (SIMD Pattern)

## 什么是 SIMD？

SIMD（Single Instruction, Multiple Data）是一条 CPU 指令集（如 x86 的 SSE/AVX、ARM 的 NEON），让一条指令同时处理多个数据——例如一次指令加 4 个 float（`__m128`）。手写 SIMD 可以显著提升矩阵运算、信号处理、游戏物理等场景的吞吐量。

> **核心思想**：把标量运算向量化，用 4× 并行吞吐换取同样多的数据。

## 为什么要手写 SIMD 而不是依赖编译器？

现代编译器（GCC/Clang -O3）会自动 vectorize 简单循环，但条件复杂、对齐不佳或循环体不规则时，编译器会放弃 auto-vectorization。手写 `#pragma omp simd` 或用 intrinsics 可以强制 SIMD 路径，保证性能不退化。

## 基础：手写出向量化加法

```cpp
#include <vector>
#include <cstddef>
#include <iostream>

std::vector<float> add(const std::vector<float>& a, const std::vector<float>& b) {
    std::vector<float> result(a.size());

    // 向量化循环：每次处理 4 个 float（对齐到 16 字节边界）
    std::size_t i = 0;
    for (; i + 4 <= a.size(); i += 4) {
        result[i]     = a[i]     + b[i];
        result[i + 1] = a[i + 1] + b[i + 1];
        result[i + 2] = a[i + 2] + b[i + 2];
        result[i + 3] = a[i + 3] + b[i + 3];
    }
    // 剩余标量处理
    for (; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}
```

核心技巧：**向量化循环** + **标量尾部**。`i + 4 <= size` 确保向量循环不越界，末尾用标量兜底。

## Dot Product：并行累加

点积是典型 SIMD 友好算法——每个 SIMD  lane 独立累加后再 sum：

```cpp
float dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
    float sums[4] = {0, 0, 0, 0};   // 4 个 lane 独立累加
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

## Lerp（线性插值）：GPU 常用

```cpp
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

## 使用 `<immintrin.h>` / `<arm_neon.h>` 的内建函数

更精细的控制可以用 compiler intrinsics（x86 SSE/AVX）：

```cpp
#ifdef __x86_64__
#include <immintrin.h>

void add_with_intrinsics(const float* __restrict__ a,
                         const float* __restrict__ b,
                         float* __restrict__ out,
                         std::size_t n) {
    std::size_t i = 0;
    // AVX: 每次 8 个 float（256 位）
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(out + i, vr);
    }
    // 标量尾部
    for (; i < n; ++i)
        out[i] = a[i] + b[i];
}
#endif
```

`_mm256_loadu_ps` / `_mm256_storeu_ps` 是不对齐加载/存储的 AVX 版本（对齐版本 `_mm256_load_ps` 更快，但要求 32 字节对齐）。

## 对齐与 `alignas`

```cpp
// 确保 32 字节对齐，适合 AVX 对齐指令
alignas(32) float aligned_a[1024];
alignas(32) float aligned_b[1024];
alignas(32) float aligned_result[1024];
```

对齐的好处：编译器可以用 `_mm256_load_ps`（比 `_mm256_loadu_ps` 快约 10-20%），减少 cache miss。

## 最佳实践

1. **先跑标量版，再向量化**：性能调优的基础是正确性，先确保标量版本通过测试
2. **向量化循环的条件是连续的内存访问**：struct of arrays（SoA）比 array of structs（AoS）更适合 SIMD
3. **不要过度优化小数据量**：向量化开销是固定常量，数据量小于 1000 个元素时标量版反而更快
4. **用 `-fopt-info-vec` 检查编译器是否 auto-vectorize**：`g++ -O3 -fopt-info-vec-optimized file.cpp` 输出 vectorization 报告
5. **对齐比 intrinsics 优先**：先尝试 `alignas` + `-O3`，编译器会自动 vectorize 很多简单循环

## 总结

手写 SIMD 的核心模式是 **"向量化循环 + 标量尾部"**，用 `i + 4 <= size` 保证向量化步长不超过边界。`alignas` 对齐内存是释放 performance 的第一步；intrinsics 是精细控制的手段，不是日常首选——大多数场景 `-O3` 就够用。
