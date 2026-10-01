// examples/deducing-this.cpp
// Demonstrates C++26 explicit object parameter ("deducing this"): one
// member function definition that covers ref-qualified overloads and
// CRTP-style fluent interfaces without duplication.
// Compile: g++ -std=c++26 -Wall -Wextra -o deducing-this deducing-this.cpp

#include <iostream>
#include <string>
#include <utility>

namespace deducing_this {

// Before C++26 you needed THREE overloads (&, const&, &&) or CRTP.
// With explicit object parameter, one definition covers all.
class Buffer {
public:
    explicit Buffer(std::string data) : data_(std::move(data)) {}

    // `self` is deduced as Buffer&, const Buffer&, or Buffer&& depending
    // on the caller. Return type tracks the value category via decltype(self).
    template <typename Self>
    auto&& append(this Self&& self, const std::string& extra) {
        self.data_ += extra;
        return std::forward<Self>(self);
    }

    template <typename Self>
    auto&& shrink(this Self&& self, std::size_t n) {
        if (self.data_.size() > n) self.data_.resize(n);
        return std::forward<Self>(self);
    }

    // const-correctness in ONE definition: const Self applies when called
    // on a const object; non-const body only compiles for mutable self.
    template <typename Self>
    auto view(this const Self& self) {
        return self.data_;  // copy out; works on const and non-const
    }

    const std::string& str() const { return data_; }

private:
    std::string data_;
};

// Explicit object parameters are only allowed in non-static member
// functions - a free function like `shout` cannot use them.

} // namespace deducing_this

int main() {
    using namespace deducing_this;
    std::cout << "=== C++26 deducing this Demo ===\n\n";

    // 1. Same append() works on lvalues, rvalues, and const objects
    std::cout << "1. one definition, three value categories:\n";
    Buffer b("abc");
    b.append("-lvalue").append("-again");          // Buffer&
    std::cout << "  lvalue chain : " << b.view() << "\n";

    Buffer("temp").append("-rvalue");              // Buffer&& (temporary)
    std::cout << "  rvalue call  : " << Buffer("temp").append("-rvalue").view() << "\n";

    const Buffer cb("const-data");
    std::cout << "  const call   : " << cb.view() << "\n";

    // 2. Fluent interface without writing &/&& overload pairs
    std::cout << "\n2. fluent chaining (append then shrink):\n";
    auto result = Buffer("0123456789")
                      .append("XYZ")
                      .shrink(5)
                      .view();
    std::cout << "  \"0123456789\" + \"XYZ\" then shrink(5): " << result << "\n";

    // 3. The return value category is preserved (no dangling copies)
    std::cout << "\n3. intermediate results stay valid through the chain\n";
    std::cout << "  b still valid: " << b.view() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
