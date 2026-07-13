#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

namespace strategy_selector {

// --- Strategy Interface ---

class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
    virtual std::string name() const = 0;
};

// --- Concrete Strategies ---

class BubbleStrategy : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
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

class QuickStrategy : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        if (data.size() <= 1) return;
        quickSort(data, 0, data.size() - 1);
    }
    std::string name() const override { return "QuickSort"; }

private:
    void quickSort(std::vector<int>& data, int low, int high) {
        if (low < high) {
            int pi = partition(data, low, high);
            quickSort(data, low, pi - 1);
            quickSort(data, pi + 1, high);
        }
    }

    int partition(std::vector<int>& data, int low, int high) {
        int pivot = data[high];
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

// --- Strategy Selector ---

class StrategySelector {
public:
    void registerStrategy(const std::string& name, std::shared_ptr<SortStrategy> strategy) {
        strategies_[name] = strategy;
    }

    std::shared_ptr<SortStrategy> select(const std::string& name) {
        auto it = strategies_.find(name);
        return (it != strategies_.end()) ? it->second : nullptr;
    }

    std::vector<std::string> availableStrategies() const {
        std::vector<std::string> names;
        for (const auto& [name, strategy] : strategies_) {
            names.push_back(name);
        }
        return names;
    }

private:
    std::map<std::string, std::shared_ptr<SortStrategy>> strategies_;
};

} // namespace strategy_selector
