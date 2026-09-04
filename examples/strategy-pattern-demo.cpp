// examples/strategy-pattern-demo.cpp
// Demonstrates Strategy Pattern with BubbleSort, QuickSort, InsertionSort.
// Compile: g++ -std=c++20 -o strategy-pattern-demo strategy-pattern-demo.cpp

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace strategy_pattern {

template<typename T>
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<T>& data) = 0;
};

template<typename T>
class BubbleSort : public SortStrategy<T> {
public:
    void sort(std::vector<T>& data) override {
        for (std::size_t i = 0; i < data.size(); ++i)
            for (std::size_t j = 0; j + 1 < data.size() - i; ++j)
                if (data[j] > data[j+1])
                    std::swap(data[j], data[j+1]);
    }
};

template<typename T>
class QuickSort : public SortStrategy<T> {
public:
    void sort(std::vector<T>& data) override {
        qsort(data, 0, static_cast<int>(data.size()) - 1);
    }
private:
    void qsort(std::vector<T>& a, int lo, int hi) {
        if (lo >= hi) return;
        int p = partition(a, lo, hi);
        qsort(a, lo, p - 1);
        qsort(a, p + 1, hi);
    }
    int partition(std::vector<T>& a, int lo, int hi) {
        T pivot = a[hi];
        int i = lo - 1;
        for (int j = lo; j < hi; ++j)
            if (a[j] <= pivot) std::swap(a[++i], a[j]);
        std::swap(a[++i], a[hi]);
        return i;
    }
};

template<typename T>
class InsertionSort : public SortStrategy<T> {
public:
    void sort(std::vector<T>& data) override {
        for (std::size_t i = 1; i < data.size(); ++i) {
            T key = data[i];
            int j = static_cast<int>(i) - 1;
            while (j >= 0 && data[j] > key) data[j+1] = data[j--];
            data[j+1] = key;
        }
    }
};

template<typename T>
class Sorter {
public:
    void setStrategy(std::shared_ptr<SortStrategy<T>> s) { strategy_ = std::move(s); }
    void sort(std::vector<T>& data) { strategy_->sort(data); }
private:
    std::shared_ptr<SortStrategy<T>> strategy_;
};

} // namespace strategy_pattern

int main() {
    using namespace strategy_pattern;
    std::cout << "=== Strategy Pattern Demo ===\n\n";

    Sorter<int> sorter;
    std::vector<int> data = {5, 3, 1, 4, 2};

    sorter.setStrategy(std::make_shared<BubbleSort<int>>());
    auto d1 = data; sorter.sort(d1);
    std::cout << "BubbleSort:    "; for (int n : d1) std::cout << n << " "; std::cout << "\n";

    sorter.setStrategy(std::make_shared<QuickSort<int>>());
    auto d2 = data; sorter.sort(d2);
    std::cout << "QuickSort:     "; for (int n : d2) std::cout << n << " "; std::cout << "\n";

    sorter.setStrategy(std::make_shared<InsertionSort<int>>());
    auto d3 = data; sorter.sort(d3);
    std::cout << "InsertionSort: "; for (int n : d3) std::cout << n << " "; std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
