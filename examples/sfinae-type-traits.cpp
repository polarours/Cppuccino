#include <iostream>
#include <string>
#include <type_traits>
#include <sstream>
#include <vector>

// --- detect if type has size() ---
template <typename, typename = void>
struct has_size : std::false_type {};

template <typename T>
struct has_size<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

// --- detect if type is iterable ---
template <typename, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end())
>> : std::true_type {};

// --- detect if streamable ---
template <typename, typename = void>
struct is_streamable : std::false_type {};

template <typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type {};

// --- SFINAE dispatch ---
template <typename T>
std::enable_if_t<is_streamable<T>::value, std::string>
safeStringify(const T& val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

template <typename T>
std::enable_if_t<!is_streamable<T>::value, std::string>
safeStringify(const T&) {
    return "[non-streamable]";
}

struct Widget {};

int main() {
    std::cout << "=== SFINAE & Type Traits ===\n\n";

    std::cout << std::boolalpha;
    std::cout << "int has size: " << has_size<int>::value << "\n";
    std::cout << "vector has size: " << has_size<std::vector<int>>::value << "\n";

    std::cout << "int iterable: " << is_iterable<int>::value << "\n";
    std::cout << "string iterable: " << is_iterable<std::string>::value << "\n";

    std::cout << "int streamable: " << is_streamable<int>::value << "\n";
    std::cout << "Widget streamable: " << is_streamable<Widget>::value << "\n";

    std::cout << "\nstringify(42): " << safeStringify(42) << "\n";
    std::cout << "stringify(string): " << safeStringify(std::string("hello")) << "\n";
    std::cout << "stringify(Widget): " << safeStringify(Widget{}) << "\n";

    return 0;
}
