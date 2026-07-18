#include "error_code.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_no_error() {
    error_code::OperationResult result;
    expect(result.ok(), "expected ok");
    expect(result.code() == error_code::ErrorCode::None, "expected None");
}

void test_error_code() {
    error_code::OperationResult result(error_code::ErrorCode::NotFound);
    expect(!result.ok(), "expected not ok");
    expect(result.code() == error_code::ErrorCode::NotFound, "expected NotFound");
}

void test_error_message() {
    error_code::OperationResult result(error_code::ErrorCode::InvalidArgument, "bad id");
    auto msg = result.message();
    expect(msg.find("Invalid argument") != std::string::npos, "expected error description");
    expect(msg.find("bad id") != std::string::npos, "expected detail");
}

void test_service_delete() {
    error_code::UserService service;
    service.addUser(1, "Alice");

    auto result = service.deleteUser(1);
    expect(result.ok(), "expected success");
    expect(service.userCount() == 0, "expected 0 users");
}

void test_service_not_found() {
    error_code::UserService service;
    auto result = service.deleteUser(99);
    expect(!result.ok(), "expected not found");
    expect(result.code() == error_code::ErrorCode::NotFound, "expected NotFound");
}

} // namespace

int main() {
    try {
        std::cout << "Error Code Tests:\n";
        test_no_error();
        std::cout << "  test_no_error: PASS\n";

        test_error_code();
        std::cout << "  test_error_code: PASS\n";

        test_error_message();
        std::cout << "  test_error_message: PASS\n";

        test_service_delete();
        std::cout << "  test_service_delete: PASS\n";

        test_service_not_found();
        std::cout << "  test_service_not_found: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "error_code_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nerror_code_tests passed" << std::endl;
    return 0;
}
