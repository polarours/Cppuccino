#include "observer_pipeline.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_pipeline() {
    observer_pipeline::Pipeline<int> pipeline;
    pipeline.addStage([](int x) { return x * 2; });
    pipeline.addStage([](int x) { return x + 1; });

    int result = pipeline.process(5);
    expect(result == 11, "expected 11");
}

void test_string_pipeline() {
    observer_pipeline::Pipeline<std::string> pipeline;
    pipeline.addStage(observer_pipeline::trim);
    pipeline.addStage(observer_pipeline::toUpper);

    std::string result = pipeline.process("  hello  ");
    expect(result == "HELLO", "expected HELLO");
}

void test_pipeline_chain() {
    observer_pipeline::Pipeline<int> pipeline;
    pipeline.addStage([](int x) { return x * 2; });
    pipeline.addStage([](int x) { return x * 2; });
    pipeline.addStage([](int x) { return x * 2; });

    int result = pipeline.process(1);
    expect(result == 8, "expected 8");
}

void test_stage_count() {
    observer_pipeline::Pipeline<int> pipeline;
    expect(pipeline.stageCount() == 0, "expected 0 stages");

    pipeline.addStage([](int x) { return x; });
    expect(pipeline.stageCount() == 1, "expected 1 stage");

    pipeline.addStage([](int x) { return x; });
    expect(pipeline.stageCount() == 2, "expected 2 stages");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Pipeline Tests:\n";
        test_basic_pipeline();
        std::cout << "  test_basic_pipeline: PASS\n";

        test_string_pipeline();
        std::cout << "  test_string_pipeline: PASS\n";

        test_pipeline_chain();
        std::cout << "  test_pipeline_chain: PASS\n";

        test_stage_count();
        std::cout << "  test_stage_count: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_pipeline_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_pipeline_tests passed" << std::endl;
    return 0;
}
