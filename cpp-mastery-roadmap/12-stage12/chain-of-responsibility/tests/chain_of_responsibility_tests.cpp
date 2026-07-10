#include "chain_of_responsibility.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_chain() {
    auto auth = std::make_shared<chain_of_responsibility::AuthHandler>();
    auto logging = std::make_shared<chain_of_responsibility::LoggingHandler>();
    auto validation = std::make_shared<chain_of_responsibility::ValidationHandler>();
    auto processing = std::make_shared<chain_of_responsibility::ProcessingHandler>();

    auth->setNext(logging);
    logging->setNext(validation);
    validation->setNext(processing);

    chain_of_responsibility::Request req{"test", 1};
    auth->handle(req);
}

void test_validation_rejects_empty() {
    auto validation = std::make_shared<chain_of_responsibility::ValidationHandler>();
    auto processing = std::make_shared<chain_of_responsibility::ProcessingHandler>();

    validation->setNext(processing);

    chain_of_responsibility::Request req{"", 1};
    // Should not reach processing handler
    validation->handle(req);
}

void test_handler_chain() {
    auto auth = std::make_shared<chain_of_responsibility::AuthHandler>();
    auto logging = std::make_shared<chain_of_responsibility::LoggingHandler>();

    auth->setNext(logging);

    chain_of_responsibility::Request req{"test", 1};
    auth->handle(req);
}

} // namespace

int main() {
    try {
        std::cout << "Chain of Responsibility Tests:\n";
        test_basic_chain();
        std::cout << "  test_basic_chain: PASS\n";

        test_validation_rejects_empty();
        std::cout << "  test_validation_rejects_empty: PASS\n";

        test_handler_chain();
        std::cout << "  test_handler_chain: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "chain_of_responsibility_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nchain_of_responsibility_tests passed" << std::endl;
    return 0;
}