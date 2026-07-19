#include "ci_cd_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <chrono>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_single_stage() {
    ci_cd_pattern::Pipeline pipeline;
    pipeline.addStage({"build", []() { return true; }});

    auto results = pipeline.run();
    expect(results.size() == 1, "expected 1 result");
    expect(results[0].status == ci_cd_pattern::StageStatus::Success, "expected success");
}

void test_multiple_stages() {
    ci_cd_pattern::Pipeline pipeline;
    pipeline.addStage({"build", []() { return true; }});
    pipeline.addStage({"test", []() { return true; }});
    pipeline.addStage({"deploy", []() { return true; }});

    auto results = pipeline.run();
    expect(results.size() == 3, "expected 3 results");
    expect(pipeline.allPassed(), "expected all passed");
}

void test_failure_skips_later() {
    ci_cd_pattern::Pipeline pipeline;
    pipeline.addStage({"build", []() { return false; }});
    pipeline.addStage({"test", []() { return true; }});
    pipeline.addStage({"deploy", []() { return true; }});

    auto results = pipeline.run();
    expect(results.size() == 3, "expected 3 results");
    expect(results[0].status == ci_cd_pattern::StageStatus::Failed, "expected failed");
    expect(results[1].status == ci_cd_pattern::StageStatus::Skipped, "expected skipped");
    expect(results[2].status == ci_cd_pattern::StageStatus::Skipped, "expected skipped");
    expect(!pipeline.allPassed(), "expected not all passed");
}

void test_exception_handling() {
    ci_cd_pattern::Pipeline pipeline;
    pipeline.addStage({"crash", []() -> bool {
        throw std::runtime_error("oops");
    }});

    auto results = pipeline.run();
    expect(results[0].status == ci_cd_pattern::StageStatus::Failed, "expected failed");
    expect(results[0].message.find("oops") != std::string::npos, "expected error message");
}

void test_stage_count() {
    ci_cd_pattern::Pipeline pipeline;
    pipeline.addStage({"a", []() { return true; }});
    pipeline.addStage({"b", []() { return true; }});

    expect(pipeline.stageCount() == 2, "expected 2 stages");
}

} // namespace

int main() {
    try {
        std::cout << "CI/CD Pattern Tests:\n";
        test_single_stage();
        std::cout << "  test_single_stage: PASS\n";

        test_multiple_stages();
        std::cout << "  test_multiple_stages: PASS\n";

        test_failure_skips_later();
        std::cout << "  test_failure_skips_later: PASS\n";

        test_exception_handling();
        std::cout << "  test_exception_handling: PASS\n";

        test_stage_count();
        std::cout << "  test_stage_count: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "ci_cd_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nci_cd_pattern_tests passed" << std::endl;
    return 0;
}
