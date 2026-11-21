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
        data_->items.push_back(item);
    }

    std::shared_ptr<Iterator<T>> createIterator() override {
        // The iterator owns the data, not the aggregate. It can outlive the
        // object that created it.
        return std::make_shared<ConcreteIterator>(data_);
    }

    std::size_t size() const override {
        return data_->items.size();
    }

    T getItem(std::size_t index) const {
        return data_->items.at(index);
    }

private:
    struct Data {
        std::vector<T> items;
    };

    class ConcreteIterator : public Iterator<T> {
    public:
        explicit ConcreteIterator(std::shared_ptr<Data> data)
            : data_(std::move(data)), index_(0) {}

        bool hasNext() const override {
            return index_ < data_->items.size();
        }

        T next() override {
            return data_->items.at(index_++);
        }

    private:
        std::shared_ptr<Data> data_;
        std::size_t index_;
    };

    std::shared_ptr<Data> data_ = std::make_shared<Data>();
};

} // namespace iterator_pattern
