#pragma once

#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <iostream>
#include <chrono>

namespace command_history {

// --- Command ---

struct CommandEntry {
    std::string command;
    std::string result;
    std::chrono::steady_clock::time_point timestamp;
};

// --- History Manager ---

class HistoryManager {
public:
    explicit HistoryManager(std::size_t maxSize = 100) : maxSize_(maxSize) {}

    void add(const std::string& command, const std::string& result = "") {
        history_.push_back({command, result, std::chrono::steady_clock::now()});
        if (history_.size() > maxSize_) {
            history_.pop_front();
        }
    }

    std::vector<CommandEntry> getAll() const {
        return std::vector<CommandEntry>(history_.begin(), history_.end());
    }

    CommandEntry getLast() const {
        if (history_.empty()) {
            return {"", "", std::chrono::steady_clock::now()};
        }
        return history_.back();
    }

    std::size_t size() const {
        return history_.size();
    }

    void clear() {
        history_.clear();
    }

    void undo() {
        if (!history_.empty()) {
            history_.pop_back();
        }
    }

private:
    std::deque<CommandEntry> history_;
    std::size_t maxSize_;
};

} // namespace command_history
