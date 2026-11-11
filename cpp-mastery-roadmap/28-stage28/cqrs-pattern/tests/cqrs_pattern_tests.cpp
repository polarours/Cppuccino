#include "cqrs_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_create_via_command() {
    cqrs_pattern::AccountService service;
    service.commands().dispatch("create", "Alice");

    auto result = service.queries().query("get", "1");
    expect(result.find("Alice") != std::string::npos, "expected Alice");
}

void test_deposit_via_command() {
    cqrs_pattern::AccountService service;
    service.commands().dispatch("create", "Alice");
    service.commands().dispatch("deposit", "1:100.50");

    auto result = service.queries().query("get", "1");
    expect(result.find("100.5") != std::string::npos, "expected 100.5 balance");
}

void test_query_list() {
    cqrs_pattern::AccountService service;
    service.commands().dispatch("create", "Alice");
    service.commands().dispatch("create", "Bob");

    auto result = service.queries().query("list", "");
    expect(result.find("Alice") != std::string::npos, "expected Alice");
    expect(result.find("Bob") != std::string::npos, "expected Bob");
}

void test_separation() {
    cqrs_pattern::AccountService service;

    service.commands().dispatch("create", "Charlie");
    service.commands().dispatch("deposit", "1:500");

    auto balance = service.queries().query("get", "1");
    expect(balance.find("500") != std::string::npos, "expected 500");
}

// Regression: the number functions of the standard library stop at the
// first character they cannot read, so "1abc" was read as 1 and the rest
// of the text was dropped without a word.
void test_a_payload_with_garbage_behind_the_number_is_refused() {
    cqrs_pattern::AccountService service;
    service.commands().dispatch("create", "Alice");

    bool threw = false;
    try {
        service.commands().dispatch("deposit", "1abc:100.50");
    } catch (const std::exception&) {
        threw = true;
    }
    expect(threw, "expected the garbage behind the number to be refused");

    threw = false;
    try {
        service.commands().dispatch("deposit", "1:100.50xyz");
    } catch (const std::exception&) {
        threw = true;
    }
    expect(threw, "expected the garbage behind the amount to be refused");
}

} // namespace

int main() {
    try {
        std::cout << "CQRS Pattern Tests:\n";
        test_create_via_command();
        std::cout << "  test_create_via_command: PASS\n";

        test_deposit_via_command();
        std::cout << "  test_deposit_via_command: PASS\n";

        test_query_list();
        std::cout << "  test_query_list: PASS\n";

        test_separation();
        std::cout << "  test_separation: PASS\n";

        test_a_payload_with_garbage_behind_the_number_is_refused();
        std::cout << "  test_a_payload_with_garbage_behind_the_number_is_refused: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "cqrs_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\ncqrs_pattern_tests passed" << std::endl;
    return 0;
}
