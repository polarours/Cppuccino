#include "strategy_pattern.hpp"

#include <iostream>
#include <vector>

int main() {
    std::cout << "=== Strategy Pattern Demo ===\n\n";

    strategy_pattern::Sorter<int> sorter;

    std::vector<int> data = {5, 3, 1, 4, 2};

    // Bubble Sort
    sorter.setStrategy(std::make_shared<strategy_pattern::BubbleSort<int>>());
    auto data1 = data;
    sorter.sort(data1);
    std::cout << "BubbleSort: ";
    for (int n : data1) std::cout << n << " ";
    std::cout << "\n";

    // Quick Sort
    sorter.setStrategy(std::make_shared<strategy_pattern::QuickSort<int>>());
    auto data2 = data;
    sorter.sort(data2);
    std::cout << "QuickSort: ";
    for (int n : data2) std::cout << n << " ";
    std::cout << "\n";

    // Insertion Sort
    sorter.setStrategy(std::make_shared<strategy_pattern::InsertionSort<int>>());
    auto data3 = data;
    sorter.sort(data3);
    std::cout << "InsertionSort: ";
    for (int n : data3) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
