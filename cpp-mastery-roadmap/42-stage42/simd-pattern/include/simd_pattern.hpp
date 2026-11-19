#pragma once

#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>

namespace simd_pattern {

class VectorMath {
public:
    // The element-wise operations read both vectors at the same index, so a
    // shorter operand limits the result instead of reading past its end.
    static std::size_t common(const std::vector<float>& a, const std::vector<float>& b) {
        return a.size() < b.size() ? a.size() : b.size();
    }

    static std::vector<float> add(const std::vector<float>& a, const std::vector<float>& b) {
        const std::size_t n = common(a, b);
        std::vector<float> result(n);
        std::size_t i = 0;
        for (; i + 4 <= n; i += 4) {
            result[i]     = a[i]     + b[i];
            result[i + 1] = a[i + 1] + b[i + 1];
            result[i + 2] = a[i + 2] + b[i + 2];
            result[i + 3] = a[i + 3] + b[i + 3];
        }
        for (; i < n; ++i) {
            result[i] = a[i] + b[i];
        }
        return result;
    }

    static std::vector<float> multiply(const std::vector<float>& a, const std::vector<float>& b) {
        const std::size_t n = common(a, b);
        std::vector<float> result(n);
        std::size_t i = 0;
        for (; i + 4 <= n; i += 4) {
            result[i]     = a[i]     * b[i];
            result[i + 1] = a[i + 1] * b[i + 1];
            result[i + 2] = a[i + 2] * b[i + 2];
            result[i + 3] = a[i + 3] * b[i + 3];
        }
        for (; i < n; ++i) {
            result[i] = a[i] * b[i];
        }
        return result;
    }

    static float dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
        const std::size_t n = common(a, b);
        float sum = 0;
        std::size_t i = 0;
        float sums[4] = {0, 0, 0, 0};
        for (; i + 4 <= n; i += 4) {
            sums[0] += a[i]     * b[i];
            sums[1] += a[i + 1] * b[i + 1];
            sums[2] += a[i + 2] * b[i + 2];
            sums[3] += a[i + 3] * b[i + 3];
        }
        sum = sums[0] + sums[1] + sums[2] + sums[3];
        for (; i < n; ++i) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    static std::vector<float> scalarMultiply(const std::vector<float>& a, float scalar) {
        std::vector<float> result(a.size());
        std::size_t i = 0;
        for (; i + 4 <= a.size(); i += 4) {
            result[i]     = a[i]     * scalar;
            result[i + 1] = a[i + 1] * scalar;
            result[i + 2] = a[i + 2] * scalar;
            result[i + 3] = a[i + 3] * scalar;
        }
        for (; i < a.size(); ++i) {
            result[i] = a[i] * scalar;
        }
        return result;
    }

    static std::vector<float> lerp(const std::vector<float>& a, const std::vector<float>& b, float t) {
        const std::size_t n = common(a, b);
        std::vector<float> result(n);
        float t1 = 1.0f - t;
        std::size_t i = 0;
        for (; i + 4 <= n; i += 4) {
            result[i]     = a[i]     * t1 + b[i]     * t;
            result[i + 1] = a[i + 1] * t1 + b[i + 1] * t;
            result[i + 2] = a[i + 2] * t1 + b[i + 2] * t;
            result[i + 3] = a[i + 3] * t1 + b[i + 3] * t;
        }
        for (; i < n; ++i) {
            result[i] = a[i] * t1 + b[i] * t;
        }
        return result;
    }
};

} // namespace simd_pattern
