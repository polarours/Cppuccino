#include "strategy_adapter.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_adapter_sorts() {
    auto legacy = std::make_shared<strategy_adapter::LegacySorter>();
    auto adapter = std::make_shared<strategy_adapter::LegacySorterAdapter>(legacy);

    std::vector<int> data = {5, 3, 1, 4, 2};
    adapter->sort(data);

    expect(data[0] == 1, "expected 1");
    expect(data[4] == 5, "expected 5");
}

void test_adapter_interface() {
    auto legacy = std::make_shared<strategy_adapter::LegacySorter>();
    auto adapter = std::make_shared<strategy_adapter::LegacySorterAdapter>(legacy);

    // Use through modern interface
    std::vector<int> data = {10, 5, 15, 20};
    adapter->sort(data);

    expect(data[0] == 5, "expected 5");
    expect(data[3] == 20, "expected 20");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Adapter Tests:\n";
        test_adapter_sorts();
        std::cout << "  test_adapter_sorts: PASS\n";

        test_adapter_interface();
        std::cout << "  test_adapter_interface: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_adapter_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_adapter_tests passed" << std::endl;
    return 0;
}
