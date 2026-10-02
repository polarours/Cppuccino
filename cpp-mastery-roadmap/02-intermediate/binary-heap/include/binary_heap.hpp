#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace binary_heap {

// Binary heap (array-backed) with a custom comparator.
// Default Compare = std::less => max-heap; use std::greater for min-heap.
template <typename T, typename Compare = std::less<T>>
class BinaryHeap {
public:
    explicit BinaryHeap(Compare comp = Compare{}) : comp_(comp) {}

    void push(T value) {
        data_.push_back(std::move(value));
        siftUp(data_.size() - 1);
    }

    // Extract the top element; throws when empty.
    T pop() {
        if (data_.empty()) throw std::runtime_error("pop from empty heap");
        T top = std::move(data_.front());
        std::swap(data_.front(), data_.back());
        data_.pop_back();
        if (!data_.empty()) siftDown(0);
        return top;
    }

    const T& top() const {
        if (data_.empty()) throw std::runtime_error("top of empty heap");
        return data_.front();
    }

    bool empty() const { return data_.empty(); }
    std::size_t size() const { return data_.size(); }

    // O(n) bottom-up heapify (faster than n pushes)
    void heapify(std::vector<T> items) {
        data_ = std::move(items);
        if (data_.size() < 2) return;
        for (std::size_t i = data_.size() / 2; i-- > 0;) siftDown(i);
    }

private:
    static std::size_t parent(std::size_t i) { return (i - 1) / 2; }
    static std::size_t left(std::size_t i) { return 2 * i + 1; }
    static std::size_t right(std::size_t i) { return 2 * i + 2; }

    // True when a should be ABOVE b in the heap order.
    // Standard heap convention: with comp = std::less (max-heap), the child
    // rises when comp(parent, child) holds, i.e. parent < child.
    bool above(const T& a, const T& b) const { return comp_(b, a); }

    void siftUp(std::size_t i) {
        while (i > 0) {
            std::size_t p = parent(i);
            if (!above(data_[i], data_[p])) break;
            std::swap(data_[i], data_[p]);
            i = p;
        }
    }

    void siftDown(std::size_t i) {
        const std::size_t n = data_.size();
        while (true) {
            std::size_t l = left(i), r = right(i), best = i;
            if (l < n && above(data_[l], data_[best])) best = l;
            if (r < n && above(data_[r], data_[best])) best = r;
            if (best == i) break;
            std::swap(data_[i], data_[best]);
            i = best;
        }
    }

    std::vector<T> data_;
    Compare comp_;
};

}  // namespace binary_heap
