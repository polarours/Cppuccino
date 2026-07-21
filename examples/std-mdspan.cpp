#include <mdspan>
#include <vector>
#include <iostream>
#include <numeric>

// std::mdspan - multidimensional array view (C++23)
// <mdspan>

int main() {
    std::cout << "=== std::mdspan (C++23) ===\n\n";

    // 1D mdspan
    std::vector<int> data1d = {1, 2, 3, 4, 5, 6};
    std::mdspan<int, std::dextents<size_t, 1>> span1d(data1d.data(), 6);

    std::cout << "1D span (6 elements): ";
    for (std::size_t i = 0; i < span1d.extent(0); ++i) {
        std::cout << span1d[i] << " ";
    }
    std::cout << "\n";

    // 2D mdspan (matrix)
    std::vector<int> data2d = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::mdspan<int, std::dextents<size_t, 2>> matrix(data2d.data(), 3, 3);

    std::cout << "\n3x3 matrix:\n";
    for (std::size_t i = 0; i < matrix.extent(0); ++i) {
        std::cout << "  ";
        for (std::size_t j = 0; j < matrix.extent(1); ++j) {
            std::cout << matrix[i, j] << " ";
        }
        std::cout << "\n";
    }

    // Modify through mdspan
    matrix[1, 1] = 99;
    std::cout << "\nAfter matrix[1,1] = 99:\n";
    for (std::size_t i = 0; i < matrix.extent(0); ++i) {
        std::cout << "  ";
        for (std::size_t j = 0; j < matrix.extent(1); ++j) {
            std::cout << matrix[i, j] << " ";
        }
        std::cout << "\n";
    }

    // 3D mdspan
    std::vector<int> data3d(24);
    std::iota(data3d.begin(), data3d.end(), 1);
    std::mdspan<int, std::dextents<size_t, 3>> cube(data3d.data(), 2, 3, 4);

    std::cout << "\n2x3x4 cube total elements: " << cube.size() << "\n";
    std::cout << "cube[1,2,3] = " << cube[1, 2, 3] << "\n";  // 24

    // Dynamic extents
    std::vector<int> dynamic = {10, 20, 30, 40, 50, 60};
    std::mdspan<int, std::dextents<size_t, 2>> dyn(dynamic.data(), 2, 3);

    std::cout << "\nDynamic 2x3:\n";
    for (std::size_t i = 0; i < dyn.extent(0); ++i) {
        std::cout << "  ";
        for (std::size_t j = 0; j < dyn.extent(1); ++j) {
            std::cout << dyn[i, j] << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
