#include "pipeline.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

std::vector<int> collect(auto&& gen) {
    std::vector<int> out;
    for (int v : gen) out.push_back(v);
    return out;
}

void test_iota() {
    expect((collect(pipeline::iota(3, 7)) == std::vector<int>{3, 4, 5, 6}),
           "half-open range");
    expect(collect(pipeline::iota(5, 5)).empty(), "empty range");
}

void test_filter() {
    auto g = pipeline::filter(pipeline::iota(0, 10),
                              [](int v) { return v % 3 == 0; });
    expect((collect(std::move(g)) == std::vector<int>{0, 3, 6, 9}), "divisible by 3");
}

void test_transform() {
    auto g = pipeline::transform(pipeline::iota(1, 4),
                                 [](int v) { return v * 10; });
    expect((collect(std::move(g)) == std::vector<int>{10, 20, 30}), "mapped");
}

void test_take_stops_early() {
    auto g = pipeline::take(pipeline::iota(0, 1000), 4);
    expect((collect(std::move(g)) == std::vector<int>{0, 1, 2, 3}), "only 4");
}

void test_fold() {
    int sum = pipeline::fold(pipeline::iota(1, 101), 0,
                             [](int a, int b) { return a + b; });
    expect(sum == 5050, "Gauss sum 5050");
}

void test_full_composition() {
    auto g = pipeline::take(
        pipeline::transform(
            pipeline::filter(pipeline::iota(0, 50),
                             [](int v) { return v % 4 == 0; }),
            [](int v) { return v + 1; }),
        3);
    // 0,4,8 -> +1 -> 1,5,9
    expect((collect(std::move(g)) == std::vector<int>{1, 5, 9}), "composed pipeline");
}

void test_lazy_no_work_before_iteration() {
    int produced = 0;
    auto counting = pipeline::transform(pipeline::iota(0, 100), [&](int v) {
        ++produced;
        return v;
    });
    // nothing consumed yet -> nothing produced
    expect(produced == 0, "transform body not run before iteration");
    auto taken = pipeline::take(std::move(counting), 3);
    expect((collect(std::move(taken)) == std::vector<int>{0, 1, 2}),
           "take yields 3");
    expect(produced == 3, "upstream ran exactly 3 times (early exit works)");
}

void test_generator_type_changes_through_transform() {
    auto g = pipeline::transform(pipeline::iota(1, 3),
                                 [](int v) { return std::to_string(v); });
    static_assert(std::is_same_v<decltype(g), std::generator<std::string>>,
                  "transform computes the output type");
    std::vector<std::string> out;
    for (auto&& s : g) out.push_back(std::move(s));
    expect((out == std::vector<std::string>{"1", "2"}), "string pipeline");
}

}  // namespace

int main() {
    try {
        test_iota();
        test_filter();
        test_transform();
        test_take_stops_early();
        test_fold();
        test_full_composition();
        test_lazy_no_work_before_iteration();
        test_generator_type_changes_through_transform();
        std::cout << "All pipeline tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
