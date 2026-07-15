#include "facade_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_start_sequence() {
    facade_pattern::ComputerFacade computer;
    auto log = computer.start();

    expect(log.size() == 5, "expected 5 steps");
    expect(log[0].find("freeze") != std::string::npos, "expected freeze first");
    expect(log[4].find("execute") != std::string::npos, "expected execute last");
}

void test_shutdown() {
    facade_pattern::ComputerFacade computer;
    auto log = computer.shutdown();

    expect(log.size() == 2, "expected 2 shutdown steps");
    expect(log[0].find("shutdown") != std::string::npos, "expected shutdown");
}

void test_facade_simplifies() {
    facade_pattern::ComputerFacade computer;

    auto startLog = computer.start();
    expect(!startLog.empty(), "expected non-empty start log");

    auto shutLog = computer.shutdown();
    expect(!shutLog.empty(), "expected non-empty shutdown log");
}

} // namespace

int main() {
    try {
        std::cout << "Facade Pattern Tests:\n";
        test_start_sequence();
        std::cout << "  test_start_sequence: PASS\n";

        test_shutdown();
        std::cout << "  test_shutdown: PASS\n";

        test_facade_simplifies();
        std::cout << "  test_facade_simplifies: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "facade_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nfacade_pattern_tests passed" << std::endl;
    return 0;
}
