#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <chrono>

namespace strategy_decorator {

class Strategy {
public:
    virtual ~Strategy() = default;
    virtual int execute(int value) = 0;
    virtual std::string name() const = 0;
};

class DoubleStrategy : public Strategy {
public:
    int execute(int value) override { return value * 2; }
    std::string name() const override { return "Double"; }
};

class IncrementStrategy : public Strategy {
public:
    int execute(int value) override { return value + 1; }
    std::string name() const override { return "Increment"; }
};

class LoggingDecorator : public Strategy {
public:
    explicit LoggingDecorator(std::shared_ptr<Strategy> inner)
        : inner_(inner) {}

    int execute(int value) override {
        std::cout << "[" << inner_->name() << "] input: " << value;
        int result = inner_->execute(value);
        std::cout << " output: " << result << "\n";
        return result;
    }

    std::string name() const override {
        return "Logging(" + inner_->name() + ")";
    }

private:
    std::shared_ptr<Strategy> inner_;
};

class TimingDecorator : public Strategy {
public:
    explicit TimingDecorator(std::shared_ptr<Strategy> inner)
        : inner_(inner) {}

    int execute(int value) override {
        auto start = std::chrono::steady_clock::now();
        int result = inner_->execute(value);
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        lastDurationUs_ = duration.count();
        return result;
    }

    std::string name() const override {
        return "Timing(" + inner_->name() + ")";
    }

    long long lastDurationUs() const { return lastDurationUs_; }

private:
    std::shared_ptr<Strategy> inner_;
    long long lastDurationUs_ = 0;
};

} // namespace strategy_decorator
