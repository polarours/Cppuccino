#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>

namespace strategy_adapter {

// --- Legacy Interface ---

class LegacySorter {
public:
    void sortArray(int* arr, int size) {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size - 1 - i; ++j) {
                if (arr[j] > arr[j + 1]) {
                    std::swap(arr[j], arr[j + 1]);
                }
            }
        }
    }
};

// --- Modern Interface ---

class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
};

// --- Adapter ---

class LegacySorterAdapter : public SortStrategy {
public:
    explicit LegacySorterAdapter(std::shared_ptr<LegacySorter> legacy)
        : legacy_(legacy) {}

    void sort(std::vector<int>& data) override {
        legacy_->sortArray(data.data(), data.size());
    }

private:
    std::shared_ptr<LegacySorter> legacy_;
};

} // namespace strategy_adapter
