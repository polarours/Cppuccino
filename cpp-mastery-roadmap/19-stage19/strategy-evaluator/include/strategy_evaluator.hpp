#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <chrono>

namespace strategy_evaluator {

// --- Strategy Interface ---

class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
    virtual std::string name() const = 0;
};

// --- Concrete Strategies ---

class BubbleSort : public SortStrategy {
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

class QuickSort : public SortStrategy {
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

// --- Evaluator ---

struct EvaluationResult {
    std::string strategyName;
    double timeMs;
    bool correct;
};

class StrategyEvaluator {
public:
    void evaluate(std::shared_ptr<SortStrategy> strategy,
                  const std::vector<int>& inputData) {
        auto data = inputData;
        auto start = std::chrono::steady_clock::now();
        strategy->sort(data);
        auto end = std::chrono::steady_clock::now();

        double timeMs = std::chrono::duration<double, std::milli>(end - start).count();
        bool correct = std::is_sorted(data.begin(), data.end());

        results_.push_back({strategy->name(), timeMs, correct});
    }

    void printResults() const {
        std::cout << "Strategy Evaluation Results:\n";
        for (const auto& result : results_) {
            std::cout << "  " << result.strategyName
                      << ": " << result.timeMs << "ms"
                      << " (correct: " << (result.correct ? "yes" : "no") << ")\n";
        }
    }

    std::string getBestStrategy() const {
        if (results_.empty()) return "";

        auto best = std::min_element(results_.begin(), results_.end(),
            [](const EvaluationResult& a, const EvaluationResult& b) {
                return a.timeMs < b.timeMs;
            });
        return best->strategyName;
    }

    void clear() { results_.clear(); }

private:
    std::vector<EvaluationResult> results_;
};

} // namespace strategy_evaluator
