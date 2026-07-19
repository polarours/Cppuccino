#include "std_print.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_log_message() {
    std_print::Logger logger;
    logger.info("Server started on port ", 8080);

    expect(logger.messageCount() == 1, "expected 1 message");
    expect(logger.output()[0].find("INFO") != std::string::npos, "expected INFO");
    expect(logger.output()[0].find("8080") != std::string::npos, "expected port 8080");
}

void test_level_filtering() {
    std_print::Logger logger;
    logger.setLevel(std_print::Logger::Level::Warning);

    logger.debug("should not appear");
    logger.info("should not appear");
    logger.warn("visible warning");
    logger.error("visible error");

    expect(logger.messageCount() == 2, "expected 2 messages (warn + error)");
}

void test_multiple_args() {
    std_print::Logger logger;
    logger.info("User ", "Alice", " logged in from ", "192.168.1.1");

    expect(logger.messageCount() == 1, "expected 1");
    expect(logger.output()[0].find("Alice") != std::string::npos, "expected Alice");
    expect(logger.output()[0].find("192.168.1.1") != std::string::npos, "expected IP");
}

void test_clear() {
    std_print::Logger logger;
    logger.info("msg1");
    logger.info("msg2");
    logger.clear();

    expect(logger.messageCount() == 0, "expected 0 after clear");
}

void test_level_names() {
    std_print::Logger logger;
    logger.setLevel(std_print::Logger::Level::Debug);

    logger.debug("d");
    logger.info("i");
    logger.warn("w");
    logger.error("e");

    expect(logger.output()[0].find("DEBUG") != std::string::npos, "expected DEBUG");
    expect(logger.output()[1].find("INFO") != std::string::npos, "expected INFO");
    expect(logger.output()[2].find("WARN") != std::string::npos, "expected WARN");
    expect(logger.output()[3].find("ERROR") != std::string::npos, "expected ERROR");
}

} // namespace

int main() {
    try {
        std::cout << "Std Print Tests:\n";
        test_log_message();
        std::cout << "  test_log_message: PASS\n";

        test_level_filtering();
        std::cout << "  test_level_filtering: PASS\n";

        test_multiple_args();
        std::cout << "  test_multiple_args: PASS\n";

        test_clear();
        std::cout << "  test_clear: PASS\n";

        test_level_names();
        std::cout << "  test_level_names: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "std_print_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstd_print_tests passed" << std::endl;
    return 0;
}
