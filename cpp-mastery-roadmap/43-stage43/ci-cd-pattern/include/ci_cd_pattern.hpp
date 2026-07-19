#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <iostream>

namespace ci_cd_pattern {

enum class StageStatus { Pending, Running, Success, Failed, Skipped };

struct StageResult {
    std::string name;
    StageStatus status;
    std::string message;
    double durationMs = 0;
};

class PipelineStage {
public:
    PipelineStage(std::string name, std::function<bool()> action)
        : name_(std::move(name)), action_(std::move(action)) {}

    StageResult run() {
        status_ = StageStatus::Running;
        auto start = std::chrono::steady_clock::now();

        try {
            bool success = action_();
            auto end = std::chrono::steady_clock::now();
            double ms = std::chrono::duration<double, std::milli>(end - start).count();

            status_ = success ? StageStatus::Success : StageStatus::Failed;
            return {name_, status_, success ? "OK" : "FAILED", ms};
        } catch (const std::exception& e) {
            auto end = std::chrono::steady_clock::now();
            double ms = std::chrono::duration<double, std::milli>(end - start).count();
            status_ = StageStatus::Failed;
            return {name_, StageStatus::Failed, e.what(), ms};
        }
    }

    const std::string& name() const { return name_; }
    StageStatus status() const { return status_; }

private:
    std::string name_;
    std::function<bool()> action_;
    StageStatus status_ = StageStatus::Pending;
};

class Pipeline {
public:
    void addStage(PipelineStage stage) {
        stages_.push_back(std::move(stage));
    }

    std::vector<StageResult> run() {
        results_.clear();
        bool failed = false;

        for (auto& stage : stages_) {
            if (failed) {
                results_.push_back({stage.name(), StageStatus::Skipped, "Skipped", 0});
                continue;
            }

            auto result = stage.run();
            results_.push_back(result);

            if (result.status == StageStatus::Failed) {
                failed = true;
            }
        }

        return results_;
    }

    std::size_t stageCount() const { return stages_.size(); }

    bool allPassed() const {
        for (const auto& r : results_) {
            if (r.status != StageStatus::Success) return false;
        }
        return !results_.empty();
    }

    const std::vector<StageResult>& results() const { return results_; }

private:
    std::vector<PipelineStage> stages_;
    std::vector<StageResult> results_;
};

} // namespace ci_cd_pattern
