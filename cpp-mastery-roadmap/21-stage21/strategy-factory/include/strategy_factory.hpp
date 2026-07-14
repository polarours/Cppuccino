#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <vector>
#include <algorithm>

namespace strategy_factory {

class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
    virtual std::string name() const = 0;
};

class BubbleSortStrategy : public SortStrategy {
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

class QuickSortStrategy : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        if (data.size() <= 1) return;
        std::sort(data.begin(), data.end());
    }
    std::string name() const override { return "QuickSort"; }
};

using Creator = std::function<std::shared_ptr<SortStrategy>()>;

class StrategyFactory {
public:
    void registerStrategy(const std::string& name, Creator creator) {
        creators_[name] = creator;
    }

    std::shared_ptr<SortStrategy> create(const std::string& name) {
        auto it = creators_.find(name);
        if (it != creators_.end()) {
            return it->second();
        }
        return nullptr;
    }

    bool hasStrategy(const std::string& name) const {
        return creators_.find(name) != creators_.end();
    }

private:
    std::map<std::string, Creator> creators_;
};

} // namespace strategy_factory
