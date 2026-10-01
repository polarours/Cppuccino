// examples/delegation-demo.cpp
// Demonstrates the Delegation Pattern: an object forwards part of its work
// to a helper object instead of implementing everything itself.
// Compile: g++ -std=c++20 -Wall -Wextra -o delegation-demo delegation-demo.cpp

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace delegation_demo {

// Delegate interface: knows how to sort
class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual std::string name() const = 0;
    virtual void sort(std::vector<int>& data) const = 0;
};

class BubbleSort : public SortStrategy {
public:
    std::string name() const override { return "BubbleSort"; }
    void sort(std::vector<int>& data) const override {
        for (size_t i = 0; i < data.size(); ++i)
            for (size_t j = 0; j + 1 < data.size() - i; ++j)
                if (data[j] > data[j + 1]) std::swap(data[j], data[j + 1]);
    }
};

class StdSort : public SortStrategy {
public:
    std::string name() const override { return "std::sort"; }
    void sort(std::vector<int>& data) const override {
        std::sort(data.begin(), data.end());
    }
};

// Delegating class: owns the data, but sorting is "someone else's job"
class DataProcessor {
public:
    explicit DataProcessor(std::vector<int> data) : data_(std::move(data)) {}

    // Replace the delegate at runtime (delegation + strategy combined)
    void setStrategy(std::shared_ptr<const SortStrategy> strategy) {
        strategy_ = std::move(strategy);
    }

    void process() {
        if (!strategy_) {
            std::cout << "  no strategy set, skipping sort\n";
            return;
        }
        std::cout << "  delegating sort to " << strategy_->name() << "\n";
        strategy_->sort(data_);
    }

    const std::vector<int>& data() const { return data_; }

private:
    std::vector<int> data_;
    std::shared_ptr<const SortStrategy> strategy_;
};

// Self-delegation: a manager forwards a request to a report generator
class ReportGenerator {
public:
    std::string generate(const std::string& topic) const {
        return "[report] summary of " + topic;
    }
};

class Manager {
public:
    // Manager doesn't build reports itself - it delegates
    std::string handleRequest(const std::string& topic) const {
        return generator_.generate(topic);
    }

private:
    ReportGenerator generator_;
};

} // namespace delegation_demo

int main() {
    using namespace delegation_demo;
    std::cout << "=== Delegation Pattern Demo ===\n\n";

    DataProcessor proc({5, 3, 8, 1, 9});
    std::cout << "Before: ";
    for (int v : proc.data()) std::cout << v << ' ';
    std::cout << "\n";

    proc.setStrategy(std::make_shared<BubbleSort>());
    proc.process();
    std::cout << "After BubbleSort: ";
    for (int v : proc.data()) std::cout << v << ' ';
    std::cout << "\n\n";

    DataProcessor proc2({7, 2, 6, 4});
    proc2.setStrategy(std::make_shared<StdSort>());
    proc2.process();
    std::cout << "After std::sort: ";
    for (int v : proc2.data()) std::cout << v << ' ';
    std::cout << "\n\n";

    Manager mgr;
    std::cout << "Manager delegates report: " << mgr.handleRequest("Q3 revenue") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
