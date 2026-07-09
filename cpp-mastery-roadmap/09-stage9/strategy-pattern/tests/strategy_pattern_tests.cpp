#include "strategy_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void expectSorted(const std::vector<int>& data, const std::string& message) {
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i - 1] > data[i]) {
            throw std::runtime_error(message + ": " + std::to_string(data[i-1]) + " > " + std::to_string(data[i]));
        }
    }
}

void test_bubble_sort() {
    strategy_pattern::Sorter<int> sorter;
    sorter.setStrategy(std::make_shared<strategy_pattern::BubbleSort<int>>());

    std::vector<int> data = {5, 3, 1, 4, 2};
    sorter.sort(data);

    expectSorted(data, "BubbleSort");
    expect(sorter.getStrategyName() == "BubbleSort", "expected BubbleSort");
}

void test_quick_sort() {
    strategy_pattern::Sorter<int> sorter;
    sorter.setStrategy(std::make_shared<strategy_pattern::QuickSort<int>>());

    std::vector<int> data = {5, 3, 1, 4, 2};
    sorter.sort(data);

    expectSorted(data, "QuickSort");
    expect(sorter.getStrategyName() == "QuickSort", "expected QuickSort");
}

void test_insertion_sort() {
    strategy_pattern::Sorter<int> sorter;
    sorter.setStrategy(std::make_shared<strategy_pattern::InsertionSort<int>>());

    std::vector<int> data = {5, 3, 1, 4, 2};
    sorter.sort(data);

    expectSorted(data, "InsertionSort");
    expect(sorter.getStrategyName() == "InsertionSort", "expected InsertionSort");
}

void test_strategy_change() {
    strategy_pattern::Sorter<int> sorter;

    sorter.setStrategy(std::make_shared<strategy_pattern::BubbleSort<int>>());
    expect(sorter.getStrategyName() == "BubbleSort", "expected BubbleSort");

    sorter.setStrategy(std::make_shared<strategy_pattern::QuickSort<int>>());
    expect(sorter.getStrategyName() == "QuickSort", "expected QuickSort");
}

void test_empty_vector() {
    strategy_pattern::Sorter<int> sorter;
    sorter.setStrategy(std::make_shared<strategy_pattern::BubbleSort<int>>());

    std::vector<int> data;
    sorter.sort(data);

    expect(data.empty(), "expected empty vector");
}

void test_single_element() {
    strategy_pattern::Sorter<int> sorter;
    sorter.setStrategy(std::make_shared<strategy_pattern::BubbleSort<int>>());

    std::vector<int> data = {42};
    sorter.sort(data);

    expect(data.size() == 1, "expected 1 element");
    expect(data[0] == 42, "expected 42");
}

void test_already_sorted() {
    strategy_pattern::Sorter<int> sorter;
    sorter.setStrategy(std::make_shared<strategy_pattern::BubbleSort<int>>());

    std::vector<int> data = {1, 2, 3, 4, 5};
    sorter.sort(data);

    expectSorted(data, "Already sorted");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Pattern Tests:\n";
        test_bubble_sort();
        std::cout << "  test_bubble_sort: PASS\n";

        test_quick_sort();
        std::cout << "  test_quick_sort: PASS\n";

        test_insertion_sort();
        std::cout << "  test_insertion_sort: PASS\n";

        test_strategy_change();
        std::cout << "  test_strategy_change: PASS\n";

        test_empty_vector();
        std::cout << "  test_empty_vector: PASS\n";

        test_single_element();
        std::cout << "  test_single_element: PASS\n";

        test_already_sorted();
        std::cout << "  test_already_sorted: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "strategy_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_pattern_tests passed" << std::endl;
    return 0;
}
