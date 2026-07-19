#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

namespace std_print {

class Logger {
public:
    enum class Level { Debug, Info, Warning, Error };

    void setLevel(Level level) { level_ = level; }
    Level level() const { return level_; }

    template <typename... Args>
    void log(Level level, Args... args) {
        if (level < level_) return;

        std::ostringstream oss;
        oss << "[" << levelName(level) << "] ";
        appendAll(oss, args...);

        std::string msg = oss.str();
        output_.push_back(msg);
        std::cout << msg << "\n";
    }

    template <typename... Args>
    void debug(Args... args) { log(Level::Debug, args...); }

    template <typename... Args>
    void info(Args... args) { log(Level::Info, args...); }

    template <typename... Args>
    void warn(Args... args) { log(Level::Warning, args...); }

    template <typename... Args>
    void error(Args... args) { log(Level::Error, args...); }

    const std::vector<std::string>& output() const { return output_; }
    std::size_t messageCount() const { return output_.size(); }
    void clear() { output_.clear(); }

private:
    static std::string levelName(Level level) {
        switch (level) {
            case Level::Debug: return "DEBUG";
            case Level::Info: return "INFO";
            case Level::Warning: return "WARN";
            case Level::Error: return "ERROR";
        }
        return "UNKNOWN";
    }

    void appendAll(std::ostringstream&) {}

    template <typename T, typename... Rest>
    void appendAll(std::ostringstream& oss, const T& first, const Rest&... rest) {
        oss << first;
        appendAll(oss, rest...);
    }

    Level level_ = Level::Info;
    std::vector<std::string> output_;
};

} // namespace std_print
