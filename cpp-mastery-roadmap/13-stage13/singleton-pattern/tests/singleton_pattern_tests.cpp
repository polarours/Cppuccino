#include "singleton_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <thread>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_singleton_instance() {
    auto& instance1 = singleton_pattern::Logger::getInstance();
    auto& instance2 = singleton_pattern::Logger::getInstance();

    expect(&instance1 == &instance2, "expected same instance");
}

void test_singleton_logging() {
    auto& logger = singleton_pattern::Logger::getInstance();
    logger.clear();

    logger.log("test1");
    logger.log("test2");
    logger.log("test3");

    expect(logger.getLogCount() == 3, "expected 3 logs");
}

void test_config_singleton() {
    auto& config = singleton_pattern::Config::getInstance();

    config.set("host", "localhost");
    config.set("port", "8080");

    expect(config.get("host") == "localhost", "expected host");
    expect(config.get("port") == "8080", "expected port");
    expect(config.get("missing", "default") == "default", "expected default");
}

void test_singleton_thread_safety() {
    auto& logger = singleton_pattern::Logger::getInstance();
    logger.clear();

    // Multiple threads accessing singleton
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&logger, i]() {
            logger.log("Thread " + std::to_string(i));
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    expect(logger.getLogCount() == 10, "expected 10 logs");
}

} // namespace

int main() {
    try {
        std::cout << "Singleton Pattern Tests:\n";
        test_singleton_instance();
        std::cout << "  test_singleton_instance: PASS\n";

        test_singleton_logging();
        std::cout << "  test_singleton_logging: PASS\n";

        test_config_singleton();
        std::cout << "  test_config_singleton: PASS\n";

        test_singleton_thread_safety();
        std::cout << "  test_singleton_thread_safety: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "singleton_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nsingleton_pattern_tests passed" << std::endl;
    return 0;
}
