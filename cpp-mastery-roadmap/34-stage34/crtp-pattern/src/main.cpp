#include "crtp_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== CRTP Pattern Demo ===\n\n";

    crtp_pattern::Dog::reset();
    crtp_pattern::Dog d1("Rex");
    crtp_pattern::Dog d2("Buddy");
    std::cout << "Dogs: " << crtp_pattern::Dog::count() << "\n";

    crtp_pattern::User user("Alice", 30);
    crtp_pattern::Product product("Widget", 9.99);
    std::cout << user.print() << "\n";
    std::cout << product.print() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
