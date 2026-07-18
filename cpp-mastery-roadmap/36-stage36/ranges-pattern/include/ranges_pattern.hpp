#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iostream>

namespace ranges_pattern {

class Range {
public:
    explicit Range(std::vector<int> data) : data_(std::move(data)) {}

    Range filter(std::function<bool(int)> pred) {
        std::vector<int> result;
        std::copy_if(data_.begin(), data_.end(), std::back_inserter(result), pred);
        return Range(std::move(result));
    }

    Range map(std::function<int(int)> transform) {
        std::vector<int> result;
        result.reserve(data_.size());
        std::transform(data_.begin(), data_.end(), std::back_inserter(result), transform);
        return Range(std::move(result));
    }

    Range take(std::size_t n) {
        std::size_t count = std::min(n, data_.size());
        return Range(std::vector<int>(data_.begin(), data_.begin() + count));
    }

    Range skip(std::size_t n) {
        if (n >= data_.size()) return Range({});
        return Range(std::vector<int>(data_.begin() + n, data_.end()));
    }

    int sum() const {
        return std::accumulate(data_.begin(), data_.end(), 0);
    }

    int min() const {
        return *std::min_element(data_.begin(), data_.end());
    }

    int max() const {
        return *std::max_element(data_.begin(), data_.end());
    }

    std::size_t count() const { return data_.size(); }

    bool any(std::function<bool(int)> pred) const {
        return std::any_of(data_.begin(), data_.end(), pred);
    }

    bool all(std::function<bool(int)> pred) const {
        return std::all_of(data_.begin(), data_.end(), pred);
    }

    const std::vector<int>& data() const { return data_; }

private:
    std::vector<int> data_;
};

} // namespace ranges_pattern
