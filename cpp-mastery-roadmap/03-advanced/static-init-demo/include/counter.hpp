#pragma once
#include <string>
#include <iostream>

class Counter {
public:
    Counter(const std::string& name) : name_(name) {
        int currentIdx = index_;
        index_;
        std::cout << "[CONSTRUCT] " << name_ << " (index=" << currentIdx << ")\n";
    }

    ~Counter() {
        std::cout << "[DESTRUCT] " << name_ << "\n";
    }

    int getValue() const { return value_; }
    void increment() { value_; }

    void checkWithOther(const Counter& other) const {
        std::cout << name_ << " compares with " << other.name_
                  << ": " << value_ << " vs " << other.value_ << "\n";
    }

private:
    std::string name_;
    int value_ = 0;
    static int index_;  // Declaration only
};

// Note: Definition of index_ goes in counter.cpp to avoid multiple definitions

