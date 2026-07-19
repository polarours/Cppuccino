#include "ci_cd_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== CI/CD Pattern Demo ===\n\n";

    ci_cd_pattern::Pipeline pipeline;
    pipeline.addStage({"build", []() { return true; }});
    pipeline.addStage({"test", []() { return true; }});
    pipeline.addStage({"lint", []() { return true; }});
    pipeline.addStage({"deploy", []() { return true; }});

    auto results = pipeline.run();

    for (const auto& r : results) {
        std::string status = (r.status == ci_cd_pattern::StageStatus::Success) ? "OK" : "FAIL";
        std::cout << "  [" << status << "] " << r.name << " (" << r.durationMs << "ms)\n";
    }

    std::cout << "\nAll passed: " << std::boolalpha << pipeline.allPassed() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
