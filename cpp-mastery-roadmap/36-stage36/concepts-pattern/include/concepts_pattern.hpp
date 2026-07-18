#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <iostream>
#include <type_traits>

namespace concepts_pattern {

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename T>
concept HasToString = requires(const T& t) {
    { t.toString() } -> std::convertible_to<std::string>;
};

template <typename T>
concept Container = requires(T t) {
    typename T::value_type;
    { t.begin() } -> std::input_or_output_iterator;
    { t.end() } -> std::input_or_output_iterator;
    { t.size() } -> std::convertible_to<std::size_t>;
};

template <Numeric T>
T sum(const std::vector<T>& values) {
    T total = T{};
    for (const auto& v : values) {
        total += v;
    }
    return total;
}

template <Numeric T>
T average(const std::vector<T>& values) {
    if (values.empty()) return T{};
    return sum(values) / static_cast<T>(values.size());
}

template <typename T>
    requires HasToString<T>
std::string describe(const T& obj) {
    return obj.toString();
}

class Point {
public:
    Point(double x, double y) : x_(x), y_(y) {}
    std::string toString() const {
        return "(" + std::to_string(x_) + ", " + std::to_string(y_) + ")";
    }
    double x() const { return x_; }
    double y() const { return y_; }
private:
    double x_, y_;
};

class NoToString {};

template <Container C>
void printContainer(const C& container) {
    bool first = true;
    for (const auto& item : container) {
        if (!first) std::cout << ", ";
        std::cout << item;
        first = false;
    }
    std::cout << "\n";
}

} // namespace concepts_pattern
