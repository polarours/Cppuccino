#include "golden_test.hpp"

#include <iostream>

int main() {
    // Simulate a CLI tool's output and check it against a golden file.
    std::string actual =
        "status: ok\n"
        "items: 3\n"
        "done\n";

    // In-repo golden lives next to the test; here we use /tmp for the demo.
    const std::string goldenPath = "/tmp/cppuccino_golden_demo.txt";
    if (const char* update = std::getenv("UPDATE_GOLDEN")) (void)update;

    // Demo: write golden if absent (first run), compare on later runs.
    if (!std::ifstream(goldenPath)) {
        golden::writeFile(goldenPath, actual);
        std::cout << "golden created: " << goldenPath << "\n";
    }
    auto report = golden::checkGolden(goldenPath, actual);
    std::cout << (report.empty() ? "GOLDEN OK\n" : report);

    // Show a diff by feeding wrong content:
    auto bad = golden::diffLines(actual, actual + "unexpected\n");
    std::cout << "sample diff for a wrong line:\n";
    for (auto& l : bad) std::cout << l << "\n";
    return 0;
}
