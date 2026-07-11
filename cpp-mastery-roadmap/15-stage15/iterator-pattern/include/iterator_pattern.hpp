#pragma once

#include <vector>
#include <stdexcept>
#include <iostream>
#include <memory>

namespace iterator_pattern {

// --- Iterator Interface ---

template <typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual bool hasNext() const = 0;
    virtual T next() = 0;
};

// --- Aggregate Interface ---

template <typename T>
class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual std::shared_ptr<Iterator<T>> createIterator() = 0;
    virtual std::size_t size() const = 0;
};

// --- Concrete Aggregate ---

template <typename T>
class ConcreteAggregate : public Aggregate<T> {
public:
    void add(T item) {
        items_.push_back(item);
    }

    std::shared_ptr<Iterator<T>> createIterator() override {
        return std::make_shared<ConcreteIterator>(this);
    }

    std::size_t size() const override {
        return items_.size();
    }

    T getItem(std::size_t index) const {
        return items_.at(index);
    }

private:
    class ConcreteIterator : public Iterator<T> {
    public:
        ConcreteIterator(ConcreteAggregate* aggregate)
            : aggregate_(aggregate), index_(0) {}

        bool hasNext() const override {
            return index_ < aggregate_->size();
        }

        T next() override {
            return aggregate_->getItem(index_++);
        }

    private:
        ConcreteAggregate* aggregate_;
        std::size_t index_;
    };

    std::vector<T> items_;
};

} // namespace iterator_pattern
