#pragma once

#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <algorithm>

namespace observer_pipeline {

// --- Pipeline Stage ---

template <typename T>
using PipelineStage = std::function<T(T)>;

// --- Pipeline ---

template <typename T>
class Pipeline {
public:
    void addStage(PipelineStage<T> stage) {
        stages_.push_back(std::move(stage));
    }

    T process(T input) const {
        T result = std::move(input);
        for (const auto& stage : stages_) {
            result = stage(result);
        }
        return result;
    }

    std::size_t stageCount() const {
        return stages_.size();
    }

private:
    std::vector<PipelineStage<T>> stages_;
};

// --- String Pipeline Helpers ---

inline std::string toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return s;
}

inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n");
    return s.substr(start, end - start + 1);
}

} // namespace observer_pipeline
