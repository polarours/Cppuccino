#include "pipeline.hpp"

#include <iostream>

int main() {
    // lazy sum: (0..100) even -> squared, take 5
    auto src = pipeline::iota(0, 100);
    auto even = pipeline::filter(std::move(src), [](int v) { return v % 2 == 0; });
    auto squared = pipeline::transform(std::move(even), [](int v) { return v * v; });
    auto firstFive = pipeline::take(std::move(squared), 5);

    std::cout << "first five even squares:";
    for (int v : firstFive) std::cout << " " << v;
    std::cout << "\n";

    // terminal fold
    auto sum = pipeline::fold(pipeline::iota(1, 11), 0,
                              [](int acc, int v) { return acc + v; });
    std::cout << "sum 1..10 = " << sum << "\n";
    return 0;
}
