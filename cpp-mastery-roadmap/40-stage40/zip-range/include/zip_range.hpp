#pragma once

#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>
#include <cstddef>

namespace zip_range {

template <typename... Containers>
class ZipRange {
public:
    explicit ZipRange(Containers&... args) {
        initPtrs(args...);
    }

    class Iterator {
    public:
        Iterator(Containers*... ptrs, std::size_t idx) : idx_(idx) {
            initPtrs(ptrs...);
        }

        auto operator*() const {
            return deref(std::index_sequence_for<Containers...>{});
        }

        Iterator& operator++() { ++idx_; return *this; }
        bool operator!=(const Iterator& other) const { return idx_ != other.idx_; }

    private:
        template <std::size_t... Is>
        auto deref(std::index_sequence<Is...>) const {
            return std::tie(std::get<Is>(ptrs_)->at(idx_)...);
        }

        void initPtrs(Containers*... ptrs) {
            ptrs_ = std::make_tuple(ptrs...);
        }

        std::tuple<Containers*...> ptrs_;
        std::size_t idx_ = 0;
    };

    Iterator begin() { return makeIter(0, std::index_sequence_for<Containers...>{}); }
    Iterator end() { return makeIter(size(), std::index_sequence_for<Containers...>{}); }

    std::size_t size() const {
        return minSize(std::index_sequence_for<Containers...>{});
    }

private:
    void initPtrs(Containers&... args) {
        ptrs_ = std::make_tuple((&args)...);
    }

    template <std::size_t... Is>
    std::size_t minSize(std::index_sequence<Is...>) const {
        std::size_t result = std::get<0>(ptrs_)->size();
        ((result = std::min(result, std::get<Is>(ptrs_)->size())), ...);
        return result;
    }

    template <std::size_t... Is>
    Iterator makeIter(std::size_t idx, std::index_sequence<Is...>) {
        return Iterator(std::get<Is>(ptrs_)..., idx);
    }

    std::tuple<Containers*...> ptrs_;
};

template <typename... Containers>
auto zip(Containers&... containers) {
    return ZipRange<Containers...>(containers...);
}

} // namespace zip_range
