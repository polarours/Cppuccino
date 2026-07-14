#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <memory>
#include <iostream>

namespace strategy_combinator {

class Predicate {
public:
    virtual ~Predicate() = default;
    virtual bool evaluate(int value) const = 0;
    virtual std::string name() const = 0;
};

class EvenPredicate : public Predicate {
public:
    bool evaluate(int value) const override { return value % 2 == 0; }
    std::string name() const override { return "Even"; }
};

class PositivePredicate : public Predicate {
public:
    bool evaluate(int value) const override { return value > 0; }
    std::string name() const override { return "Positive"; }
};

class GreaterThanPredicate : public Predicate {
public:
    explicit GreaterThanPredicate(int threshold) : threshold_(threshold) {}
    bool evaluate(int value) const override { return value > threshold_; }
    std::string name() const override { return "GreaterThan(" + std::to_string(threshold_) + ")"; }
private:
    int threshold_;
};

enum class CombineMode { All, Any };

class PredicateCombinator {
public:
    explicit PredicateCombinator(CombineMode mode = CombineMode::All) : mode_(mode) {}

    void add(std::shared_ptr<Predicate> predicate) {
        predicates_.push_back(std::move(predicate));
    }

    bool evaluate(int value) const {
        if (predicates_.empty()) return true;

        if (mode_ == CombineMode::All) {
            return std::all_of(predicates_.begin(), predicates_.end(),
                [value](const auto& p) { return p->evaluate(value); });
        } else {
            return std::any_of(predicates_.begin(), predicates_.end(),
                [value](const auto& p) { return p->evaluate(value); });
        }
    }

    std::vector<int> filter(const std::vector<int>& values) const {
        std::vector<int> result;
        for (int v : values) {
            if (evaluate(v)) result.push_back(v);
        }
        return result;
    }

    std::size_t size() const { return predicates_.size(); }

private:
    CombineMode mode_;
    std::vector<std::shared_ptr<Predicate>> predicates_;
};

} // namespace strategy_combinator
