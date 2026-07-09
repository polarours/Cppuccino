#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <algorithm>
#include <iostream>

namespace observer_pattern {

// --- Observer Interface ---

template <typename T>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const T& value) = 0;
};

// --- Subject ---

template <typename T>
class Subject {
public:
    void attach(std::shared_ptr<Observer<T>> observer) {
        observers_.push_back(observer);
    }

    void detach(std::shared_ptr<Observer<T>> observer) {
        observers_.erase(
            std::remove_if(observers_.begin(), observers_.end(),
                [&observer](const std::weak_ptr<Observer<T>>& wp) {
                    return wp.expired() || wp.lock() == observer;
                }),
            observers_.end()
        );
    }

    void notify() {
        for (auto it = observers_.begin(); it != observers_.end(); ) {
            if (auto observer = it->lock()) {
                observer->update(value_);
                ++it;
            } else {
                it = observers_.erase(it);
            }
        }
    }

    void setValue(T value) {
        value_ = std::move(value);
        notify();
    }

    T getValue() const {
        return value_;
    }

private:
    T value_;
    std::vector<std::weak_ptr<Observer<T>>> observers_;
};

// --- Concrete Observer ---

template <typename T>
class Logger : public Observer<T> {
public:
    void update(const T& value) override {
        std::cout << "[Logger] Value changed to: " << value << "\n";
    }
};

template <typename T>
class Counter : public Observer<T> {
public:
    void update(const T& value) override {
        count_++;
        std::cout << "[Counter] Update #" << count_ << "\n";
    }

    int getCount() const { return count_; }

private:
    int count_ = 0;
};

} // namespace observer_pattern
