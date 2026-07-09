#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <string>

namespace strategy_pattern {

// --- Strategy Interface ---

template <typename T>
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<T>& data) = 0;
    virtual std::string name() const = 0;
};

// --- Concrete Strategies ---

template <typename T>
class BubbleSort : public SortStrategy<T> {
public:
    void sort(std::vector<T>& data) override {
        for (size_t i = 0; i < data.size(); ++i) {
            for (size_t j = 0; j < data.size() - 1 - i; ++j) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }

    std::string name() const override { return "BubbleSort"; }
};

template <typename T>
class QuickSort : public SortStrategy<T> {
public:
    void sort(std::vector<T>& data) override {
        if (data.size() <= 1) return;
        quickSort(data, 0, data.size() - 1);
    }

    std::string name() const override { return "QuickSort"; }

private:
    void quickSort(std::vector<T>& data, int low, int high) {
        if (low < high) {
            int pi = partition(data, low, high);
            quickSort(data, low, pi - 1);
            quickSort(data, pi + 1, high);
        }
    }

    int partition(std::vector<T>& data, int low, int high) {
        T pivot = data[high];
        int i = low - 1;

        for (int j = low; j < high; ++j) {
            if (data[j] < pivot) {
                i++;
                std::swap(data[i], data[j]);
            }
        }
        std::swap(data[i + 1], data[high]);
        return i + 1;
    }
};

template <typename T>
class InsertionSort : public SortStrategy<T> {
public:
    void sort(std::vector<T>& data) override {
        for (size_t i = 1; i < data.size(); ++i) {
            T key = data[i];
            int j = i - 1;
            while (j >= 0 && data[j] > key) {
                data[j + 1] = data[j];
                j--;
            }
            data[j + 1] = key;
        }
    }

    std::string name() const override { return "InsertionSort"; }
};

// --- Context ---

template <typename T>
class Sorter {
public:
    void setStrategy(std::shared_ptr<SortStrategy<T>> strategy) {
        strategy_ = std::move(strategy);
    }

    void sort(std::vector<T>& data) {
        if (strategy_) {
            strategy_->sort(data);
        }
    }

    std::string getStrategyName() const {
        return strategy_ ? strategy_->name() : "None";
    }

private:
    std::shared_ptr<SortStrategy<T>> strategy_;
};

} // namespace strategy_pattern
