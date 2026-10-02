#pragma once

#include <cstddef>
#include <generator>
#include <type_traits>
#include <utility>

namespace pipeline {

// C++23 std::generator based lazy pipelines. Everything is pull-based:
// nothing runs until the consumer iterates, and early exit stops all
// upstream stages (no wasted work).
//
// These compose with ranges too, but are written with bare generators
// to show the coroutine mechanics directly.

// Source: integers in [from, to)
std::generator<int> iota(int from, int to);

// Transform stage: apply f to every upstream value.
template <typename In, typename F>
auto transform(std::generator<In> src, F f) -> std::generator<std::invoke_result_t<F&, In>> {
    for (auto&& v : src) {
        co_yield f(std::forward<In>(v));
    }
}

// Filter stage: keep only values passing the predicate.
template <typename In, typename Pred>
std::generator<In> filter(std::generator<In> src, Pred p) {
    for (auto&& v : src) {
        if (p(v)) co_yield std::forward<In>(v);
    }
}

// Take stage: first n values, then stop (propagates early exit upstream).
//
// Pull discipline matters here: a naive `for (v : src) { if (i++ >= n)
// co_return; co_yield v; }` pulls an (n+1)-th upstream value when the
// consumer probes past the end. We defer each pull to the START of the
// next iteration so exactly n values are ever produced upstream.
template <typename In>
std::generator<In> take(std::generator<In> src, std::size_t n) {
    if (n == 0) co_return;
    auto it = src.begin();
    auto end = src.end();
    if (it == end) co_return;
    co_yield *it;  // value 1 (pulled by begin())
    for (std::size_t i = 1; i < n; ++i) {
        ++it;                 // pull value i+1 only when we still need it
        if (it == end) co_return;
        co_yield *it;
    }
}

// Fold: terminal consumption (runs the pipeline exactly once).
template <typename In, typename T, typename F>
T fold(std::generator<In> src, T init, F f) {
    for (auto&& v : src) init = f(std::move(init), std::forward<In>(v));
    return init;
}

}  // namespace pipeline
