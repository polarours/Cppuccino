#pragma once

#include <string>
#include <type_traits>
#include <sstream>
#include <iostream>
#include <vector>

namespace sfinae_pattern {

template <typename T, typename = void>
struct is_printable : std::false_type {};

template <typename T>
struct is_printable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>
    : std::true_type {};

template <typename T>
constexpr bool is_printable_v = is_printable<T>::value;

template <typename T>
typename std::enable_if_t<is_printable_v<T>, std::string>
safePrint(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <typename T>
typename std::enable_if_t<!is_printable_v<T>, std::string>
safePrint(const T&) {
    return "[non-printable]";
}

template <typename T, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<const T&>().size())>>
    : std::true_type {};

template <typename T>
constexpr bool has_size_v = has_size<T>::value;

template <typename T>
typename std::enable_if_t<has_size_v<T>, std::size_t>
getSize(const T& container) {
    return container.size();
}

template <typename T>
typename std::enable_if_t<!has_size_v<T>, std::size_t>
getSize(const T&) {
    return 0;
}

} // namespace sfinae_pattern
