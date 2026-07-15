#include "service_locator.hpp"

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

class Logger {
public:
    std::string log(const std::string& msg) { return "LOG: " + msg; }
};

class Cache {
public:
    void set(const std::string& key, const std::string& value) { data_[key] = value; }
    std::string get(const std::string& key) const {
        auto it = data_.find(key);
        return (it != data_.end()) ? it->second : "";
    }
private:
    std::map<std::string, std::string> data_;
};

void test_register_and_get() {
    service_locator::ServiceLocator locator;
    auto logger = std::make_shared<Logger>();
    locator.registerService("logger", logger);

    auto retrieved = locator.getService<Logger>("logger");
    expect(retrieved != nullptr, "expected logger found");
    expect(retrieved->log("test") == "LOG: test", "expected LOG: test");
}

void test_has_service() {
    service_locator::ServiceLocator locator;
    locator.registerService("cache", std::make_shared<Cache>());

    expect(locator.hasService("cache"), "expected cache exists");
    expect(!locator.hasService("db"), "expected db not exists");
}

void test_remove_service() {
    service_locator::ServiceLocator locator;
    locator.registerService("logger", std::make_shared<Logger>());
    locator.removeService("logger");

    expect(!locator.hasService("logger"), "expected logger removed");
    expect(locator.serviceCount() == 0, "expected 0 services");
}

void test_multiple_services() {
    service_locator::ServiceLocator locator;
    locator.registerService("logger", std::make_shared<Logger>());
    locator.registerService("cache", std::make_shared<Cache>());

    expect(locator.serviceCount() == 2, "expected 2 services");
    expect(locator.getService<Logger>("logger") != nullptr, "expected logger");
    expect(locator.getService<Cache>("cache") != nullptr, "expected cache");
}

} // namespace

int main() {
    try {
        std::cout << "Service Locator Tests:\n";
        test_register_and_get();
        std::cout << "  test_register_and_get: PASS\n";

        test_has_service();
        std::cout << "  test_has_service: PASS\n";

        test_remove_service();
        std::cout << "  test_remove_service: PASS\n";

        test_multiple_services();
        std::cout << "  test_multiple_services: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "service_locator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nservice_locator_tests passed" << std::endl;
    return 0;
}
