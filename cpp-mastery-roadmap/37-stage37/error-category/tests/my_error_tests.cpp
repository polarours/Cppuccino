#include "my_error.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_implicit_conversion_and_category() {
    std::error_code ec = my_error::Errc::timeout;  // is_error_code_enum hook
    expect(ec.category() == my_error::category(), "category identity");
    expect(std::string(ec.category().name()) == "my_error", "name()");
    expect(ec.value() == 3, "underlying value");
}

void test_messages() {
    expect(my_error::make_error_code(my_error::Errc::ok).message() == "success",
           "ok message");
    expect(my_error::make_error_code(my_error::Errc::timeout).message()
               == "operation timed out",
           "timeout message");
    std::error_code unknown(999, my_error::category());
    expect(unknown.message().find("999") != std::string::npos,
           "unknown code still descriptive");
}

void test_condition_equivalence() {
    std::error_code ec = my_error::Errc::timeout;
    expect(ec == std::errc::timed_out, "timeout == std::errc::timed_out");
    std::error_code ec2 = my_error::Errc::permission_denied;
    expect(ec2 == std::errc::permission_denied, "permission maps");
    std::error_code ok = my_error::Errc::ok;
    expect(!ok, "ok is falsy (value 0)");
    expect(ok.value() == 0 && ok.category() == my_error::category(),
           "ok is (0, my_category): error_code==error_code compares categories "
           "strictly, so it is NOT equal to a default-constructed code");
    // ...but the CONDITION direction does normalize:
    expect(ok == std::error_condition(0, std::generic_category()),
           "ok matches generic zero condition via default_error_condition");
}

void test_works_with_system_error() {
    try {
        throw std::system_error(my_error::Errc::resource_exhausted);
    } catch (const std::system_error& e) {
        expect(e.code().category() == my_error::category(), "carried category");
        expect(e.code() == my_error::Errc::resource_exhausted, "compares to enum");
    }
}

void test_not_found_maps_to_no_such_file() {
    std::error_code ec = my_error::Errc::not_found;
    expect(ec == std::errc::no_such_file_or_directory, "not_found condition");
    expect(ec != std::errc::timed_out, "no cross-talk");
}

}  // namespace

int main() {
    try {
        test_implicit_conversion_and_category();
        test_messages();
        test_condition_equivalence();
        test_works_with_system_error();
        test_not_found_maps_to_no_such_file();
        std::cout << "All my_error tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
