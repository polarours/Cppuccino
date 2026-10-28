#include "chain_of_responsibility.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <sstream>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// The classes under test report what they do by printing, so the output
// is what the test looks at.
class CapturedOutput {
public:
    CapturedOutput() : previous_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CapturedOutput() { std::cout.rdbuf(previous_); }

    CapturedOutput(const CapturedOutput&) = delete;
    CapturedOutput& operator=(const CapturedOutput&) = delete;

    std::string text() const { return buffer_.str(); }

private:
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

void test_basic_chain() {
    auto auth = std::make_shared<chain_of_responsibility::AuthHandler>();
    auto logging = std::make_shared<chain_of_responsibility::LoggingHandler>();
    auto validation = std::make_shared<chain_of_responsibility::ValidationHandler>();
    auto processing = std::make_shared<chain_of_responsibility::ProcessingHandler>();

    auth->setNext(logging);
    logging->setNext(validation);
    validation->setNext(processing);

    chain_of_responsibility::Request req{"test", 1};
    CapturedOutput output;
    auth->handle(req);

    const std::string text = output.text();
    expect(text.find("[Auth]") != std::string::npos,
           "expected the auth handler to run: " + text);
    expect(text.find("[Log]") != std::string::npos,
           "expected the logging handler to run: " + text);
    expect(text.find("[Validation]") != std::string::npos,
           "expected the validation handler to run: " + text);
    expect(text.find("[Processing]") != std::string::npos,
           "expected the processing handler to run: " + text);

    // The chain runs in the order the handlers were linked.
    expect(text.find("[Auth]") < text.find("[Log]"),
           "expected auth before log: " + text);
    expect(text.find("[Log]") < text.find("[Validation]"),
           "expected log before validation: " + text);
    expect(text.find("[Validation]") < text.find("[Processing]"),
           "expected validation before processing: " + text);
}

void test_validation_rejects_empty() {
    auto validation = std::make_shared<chain_of_responsibility::ValidationHandler>();
    auto processing = std::make_shared<chain_of_responsibility::ProcessingHandler>();

    validation->setNext(processing);

    chain_of_responsibility::Request req{"", 1};
    CapturedOutput output;
    validation->handle(req);

    const std::string text = output.text();
    expect(text.find("Request rejected") != std::string::npos,
           "expected the request to be rejected: " + text);
    expect(text.find("[Processing]") == std::string::npos,
           "expected the processing handler not to run: " + text);
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

    } catch (const std::exception& exception) {
        std::cerr << "chain_of_responsibility_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nchain_of_responsibility_tests passed" << std::endl;
    return 0;
}