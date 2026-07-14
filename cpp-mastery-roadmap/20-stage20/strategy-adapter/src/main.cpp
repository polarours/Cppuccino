#include "strategy_adapter.hpp"

#include <iostream>
#include <memory>
#include <vector>

int main() {
    std::cout << "=== Strategy Adapter Demo ===\n\n";

    auto legacy = std::make_shared<strategy_adapter::LegacySorter>();
    auto adapter = std::make_shared<strategy_adapter::LegacySorterAdapter>(legacy);

    std::vector<int> data = {5, 3, 1, 4, 2};
    adapter->sort(data);

    std::cout << "Sorted: ";
    for (int n : data) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
