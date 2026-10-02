#include "my_error.hpp"

#include <iostream>

int main() {
    std::error_code ec = my_error::Errc::timeout;  // implicit via trait
    std::cout << "code: " << ec.value() << "\n";
    std::cout << "category: " << ec.category().name() << "\n";
    std::cout << "message: " << ec.message() << "\n";
    std::cout << "== timed_out condition: "
              << std::boolalpha << (ec == std::errc::timed_out) << "\n";

    auto ec2 = my_error::make_error_code(my_error::Errc::not_found);
    std::cout << "not_found message: " << ec2.message() << "\n";
    return 0;
}
