#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace strategy_chain {

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual bool execute(int& value) = 0;
    virtual std::string name() const = 0;
};

class DoubleStrategy : public Strategy {
public:
    bool execute(int& value) override {
        value *= 2;
        return true;
    }
    std::string name() const override { return "Double"; }
};

class IncrementStrategy : public Strategy {
public:
    bool execute(int& value) override {
        value++;
        return true;
    }
    std::string name() const override { return "Increment"; }
};

class StrategyChain {
public:
    void add(std::shared_ptr<Strategy> strategy) {
        strategies_.push_back(strategy);
    }

    int execute(int value) {
        for (auto& strategy : strategies_) {
            if (!strategy->execute(value)) {
                break;
            }
        }
        return value;
    }

    std::size_t size() const {
        return strategies_.size();
    }

private:
    std::vector<std::shared_ptr<Strategy>> strategies_;
};

} // namespace strategy_chain
