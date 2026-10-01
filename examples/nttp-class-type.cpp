// examples/nttp-class-type.cpp
// Demonstrates C++20 non-type template parameters of class type and
// template<auto> - compile-time values as types in the template argument.
// Compile: g++ -std=c++20 -Wall -Wextra -o nttp-class-type nttp-class-type.cpp

#include <array>
#include <cstddef>
#include <iostream>
#include <string_view>

namespace nttp_demo {

// 1. Classical NTTP: a size
template <std::size_t N>
struct RingBuffer {
    std::array<int, N> data{};
    std::size_t head = 0;

    void push(int v) { data[head++ % N] = v; }
    static constexpr std::size_t capacity() { return N; }
};

// 2. template<auto> - the value itself is the parameter, type deduced
template <auto Value>
struct Constant {
    static constexpr auto value = Value;
    using value_type = decltype(Value);
};

// 3. Class-type NTTP (C++20): a struct as a template argument.
//    Must be a structural type: all members public, no padding surprises.
struct Config {
    int maxRetries;
    std::size_t timeoutMs;
    bool verbose;
    // C++20 allows defaulted <=> for structural types
    auto operator<=>(const Config&) const = default;
};

template <Config C>
class HttpClient {
public:
    void describe() const {
        std::cout << "  HttpClient: retries=" << C.maxRetries
                  << " timeout=" << C.timeoutMs << "ms"
                  << " verbose=" << std::boolalpha << C.verbose << "\n";
    }
    // The config is a compile-time constant - no runtime storage
    static constexpr int retries() { return C.maxRetries; }
};

// 4. String as NTTP (C++20, via fixed string or class with char array)
template <std::size_t N>
struct FixedString {
    char data[N]{};
    constexpr FixedString(const char (&s)[N]) {
        for (std::size_t i = 0; i < N; ++i) data[i] = s[i];
    }
    constexpr std::string_view view() const { return {data, N - 1}; }
};

template <FixedString Name>
struct NamedRoute {
    static constexpr std::string_view name() { return Name.view(); }
};

} // namespace nttp_demo

int main() {
    using namespace nttp_demo;
    std::cout << "=== C++20 Class-Type NTTP Demo ===\n\n";

    // 1. Size NTTP
    std::cout << "1. size NTTP:\n";
    RingBuffer<4> rb;
    rb.push(10); rb.push(20); rb.push(30);
    std::cout << "  capacity = " << RingBuffer<4>::capacity() << "\n";

    // 2. template<auto>
    std::cout << "\n2. template<auto>:\n";
    std::cout << "  Constant<42>::value   = " << Constant<42>::value << "\n";
    std::cout << "  Constant<'X'>::value  = " << Constant<'X'>::value << "\n";
    std::cout << "  (one template, three value types: int, char, ...)\n";

    // 3. Class-type NTTP: config baked into the type
    std::cout << "\n3. class-type NTTP (Config):\n";
    constexpr Config fast{3, 500, false};
    constexpr Config debug{10, 30000, true};
    HttpClient<fast> prodClient;
    HttpClient<debug> debugClient;
    prodClient.describe();
    debugClient.describe();
    std::cout << "  compile-time retries(): " << HttpClient<fast>::retries() << "\n";
    // HttpClient<fast> and HttpClient<debug> are DIFFERENT types:
    std::cout << "  different configs = different types: "
              << std::boolalpha
              << !std::is_same_v<decltype(prodClient), decltype(debugClient)> << "\n";

    // 4. String NTTP
    std::cout << "\n4. string NTTP:\n";
    NamedRoute<"/api/v2/users"> users;
    std::cout << "  route name: " << users.name() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
