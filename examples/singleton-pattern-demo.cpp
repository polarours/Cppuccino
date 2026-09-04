// examples/singleton-pattern-demo.cpp
// Demonstrates Meyers' Singleton and thread-safe Logger/Config singletons.
// Compile: g++ -std=c++20 -o singleton-pattern-demo singleton-pattern-demo.cpp -lpthread

#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace singleton_pattern {

class Logger {
public:
    static Logger& getInstance() {
        static Logger inst;  // Meyers' Singleton — thread-safe in C++11+
        return inst;
    }

    void log(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        logs_.push_back(msg);
    }

    std::size_t getLogCount() const { return logs_.size(); }
    const std::string& getLog(std::size_t i) const { return logs_[i]; }

    // Delete copy/move
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;
    std::mutex mtx_;
    std::vector<std::string> logs_;
};

class Config {
public:
    static Config& getInstance() {
        static Config inst;
        return inst;
    }

    void set(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mtx_);
        data_[key] = value;
    }

    std::string get(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = data_.find(key);
        return it != data_.end() ? it->second : "";
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

private:
    Config() = default;
    mutable std::mutex mtx_;
    std::unordered_map<std::string, std::string> data_;
};

} // namespace singleton_pattern

int main() {
    using namespace singleton_pattern;
    std::cout << "=== Singleton Pattern Demo ===\n\n";

    auto& logger = Logger::getInstance();
    logger.log("Application started");
    logger.log("Processing data");
    logger.log("Application finished");
    std::cout << "Total logs: " << logger.getLogCount() << "\n\n";

    auto& config = Config::getInstance();
    config.set("host", "localhost");
    config.set("port", "8080");
    std::cout << "Config host: " << config.get("host") << "\n";
    std::cout << "Config port: " << config.get("port") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
