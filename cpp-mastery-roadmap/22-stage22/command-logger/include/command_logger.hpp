#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <functional>

namespace command_logger {

struct LogEntry {
    std::string command;
    std::string result;
    std::chrono::steady_clock::time_point timestamp;
    bool success;
};

class CommandLogger {
public:
    void log(const std::string& command, const std::string& result, bool success = true) {
        entries_.push_back({command, result, std::chrono::steady_clock::now(), success});
    }

    std::vector<LogEntry> getEntries() const {
        return entries_;
    }

    std::size_t size() const {
        return entries_.size();
    }

    void clear() {
        entries_.clear();
    }

    void printAll() const {
        for (const auto& entry : entries_) {
            std::cout << "[" << (entry.success ? "OK" : "FAIL") << "] "
                      << entry.command << " -> " << entry.result << "\n";
        }
    }

private:
    std::vector<LogEntry> entries_;
};

} // namespace command_logger
