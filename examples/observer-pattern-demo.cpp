// examples/observer-pattern-demo.cpp
// Demonstrates Subject-Observer with Logger and Counter observers.
// Compile: g++ -std=c++20 -o observer-pattern-demo observer-pattern-demo.cpp -lpthread

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace observer_pattern {

template<typename T>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const T& value) = 0;
};

template<typename T>
class Subject {
public:
    void attach(std::shared_ptr<Observer<T>> obs) {
        std::lock_guard<std::mutex> lock(mtx_);
        observers_.push_back(obs);
    }

    void detach(std::shared_ptr<Observer<T>> obs) {
        std::lock_guard<std::mutex> lock(mtx_);
        observers_.erase(
            std::remove_if(observers_.begin(), observers_.end(),
                [&](const auto& o){ return o.lock() == obs; }),
            observers_.end());
    }

    void setValue(const T& v) {
        std::lock_guard<std::mutex> lock(mtx_);
        value_ = v;
        for (auto& obs : observers_) {
            if (auto sp = obs.lock()) sp->update(value_);
        }
    }

private:
    T value_;
    std::mutex mtx_;
    std::vector<std::weak_ptr<Observer<T>>> observers_;
};

template<typename T>
class Logger : public Observer<T> {
public:
    void update(const T& v) override {
        std::cout << "[Logger] received: " << v << "\n";
    }
};

template<typename T>
class Counter : public Observer<T> {
public:
    void update(const T&) override { ++count_; }
    std::size_t getCount() const { return count_; }
private:
    std::size_t count_ = 0;
};

} // namespace observer_pattern

int main() {
    using namespace observer_pattern;
    std::cout << "=== Observer Pattern Demo ===\n\n";

    Subject<std::string> subject;
    auto logger  = std::make_shared<Logger<std::string>>();
    auto counter = std::make_shared<Counter<std::string>>();

    subject.attach(logger);
    subject.attach(counter);

    subject.setValue("first");
    subject.setValue("second");
    subject.setValue("third");

    std::cout << "\nTotal updates: " << counter->getCount() << "\n";
    std::cout << "=== Demo Complete ===\n";
    return 0;
}
