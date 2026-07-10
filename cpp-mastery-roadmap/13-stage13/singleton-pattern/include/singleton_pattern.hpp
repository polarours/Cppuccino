#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <mutex>
#include <map>

namespace singleton_pattern {

// --- Thread-Safe Singleton ---

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[LOG] " << message << "\n";
        count_++;
    }

    int getLogCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        count_ = 0;
    }

private:
    Logger() = default;
    ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    int count_ = 0;
    mutable std::mutex mutex_;
};

// --- Singleton with Lazy Initialization ---

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    void set(const std::string& key, const std::string& value) {
        config_[key] = value;
    }

    std::string get(const std::string& key, const std::string& defaultValue = "") const {
        auto it = config_.find(key);
        return (it != config_.end()) ? it->second : defaultValue;
    }

    bool has(const std::string& key) const {
        return config_.find(key) != config_.end();
    }

private:
    Config() = default;
    std::map<std::string, std::string> config_;
};

} // namespace singleton_pattern
