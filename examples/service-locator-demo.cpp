// examples/service-locator-demo.cpp
// Demonstrates the Service Locator Pattern: a central registry that
// supplies shared services to clients (without hard-wiring dependencies).
// Compile: g++ -std=c++20 -Wall -Wextra -o service-locator-demo service-locator-demo.cpp

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace service_locator {

// Service interfaces
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& msg) = 0;
};

class Cache {
public:
    virtual ~Cache() = default;
    virtual void put(const std::string& key, const std::string& value) = 0;
    virtual std::string get(const std::string& key) const = 0;
};

class ConsoleLogger : public Logger {
public:
    void log(const std::string& msg) override { std::cout << "[LOG] " << msg << "\n"; }
};

class MemoryCache : public Cache {
public:
    void put(const std::string& key, const std::string& value) override { data_[key] = value; }
    std::string get(const std::string& key) const override {
        auto it = data_.find(key);
        return it != data_.end() ? it->second : "";
    }
private:
    std::map<std::string, std::string> data_;
};

// The locator itself
class ServiceLocator {
public:
    template <typename T>
    static void registerService(const std::string& name,
                                std::function<std::shared_ptr<T>()> factory) {
        providers()[name] = [factory]() -> std::shared_ptr<void> {
            return factory();
        };
    }

    template <typename T>
    static std::shared_ptr<T> getService(const std::string& name) {
        auto& p = providers();
        auto it = p.find(name);
        if (it == p.end())
            throw std::runtime_error("Service not registered: " + name);
        return std::static_pointer_cast<T>(it->second());
    }

    static void reset() { providers().clear(); }

private:
    using Provider = std::function<std::shared_ptr<void>()>;
    static std::map<std::string, Provider>& providers() {
        static std::map<std::string, Provider> p;
        return p;
    }
};

// Client code asks the locator instead of constructing dependencies
class OrderService {
public:
    void placeOrder(const std::string& item) {
        auto logger = ServiceLocator::getService<Logger>("logger");
        auto cache = ServiceLocator::getService<Cache>("cache");
        cache->put("last_order", item);
        logger->log("Order placed: " + item);
    }
};

} // namespace service_locator

int main() {
    using namespace service_locator;
    std::cout << "=== Service Locator Demo ===\n\n";

    ServiceLocator::reset();
    ServiceLocator::registerService<Logger>(
        "logger", [] { return std::make_shared<ConsoleLogger>(); });
    ServiceLocator::registerService<Cache>(
        "cache", [] { return std::make_shared<MemoryCache>(); });

    OrderService service;
    service.placeOrder("C++ Primer");
    service.placeOrder("Effective Modern C++");

    auto cache = ServiceLocator::getService<Cache>("cache");
    std::cout << "last_order from cache: " << cache->get("last_order") << "\n";

    // Missing service surfaces as an exception, not a null deref
    try {
        ServiceLocator::getService<Logger>("nope");
    } catch (const std::exception& e) {
        std::cout << "Expected error: " << e.what() << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
