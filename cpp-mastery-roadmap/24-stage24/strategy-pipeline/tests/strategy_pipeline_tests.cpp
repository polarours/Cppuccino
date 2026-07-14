#include "strategy_pipeline.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_single_transform() {
    strategy_pipeline::Pipeline pipeline;
    pipeline.add(std::make_shared<strategy_pipeline::UpperTransform>());

    expect(pipeline.execute("hello") == "HELLO", "expected HELLO");
    expect(pipeline.size() == 1, "expected 1 transform");
}

void test_chained_transforms() {
    strategy_pipeline::Pipeline pipeline;
    pipeline.add(std::make_shared<strategy_pipeline::TrimTransform>());
    pipeline.add(std::make_shared<strategy_pipeline::UpperTransform>());

    expect(pipeline.execute("  hello  ") == "HELLO", "expected HELLO");
}

void test_prefix_transform() {
    strategy_pipeline::Pipeline pipeline;
    pipeline.add(std::make_shared<strategy_pipeline::PrefixTransform>("[LOG] "));
    pipeline.add(std::make_shared<strategy_pipeline::UpperTransform>());

    expect(pipeline.execute("test") == "[LOG] TEST", "expected [LOG] TEST");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Pipeline Tests:\n";
        test_single_transform();
        std::cout << "  test_single_transform: PASS\n";

        test_chained_transforms();
        std::cout << "  test_chained_transforms: PASS\n";

        test_prefix_transform();
        std::cout << "  test_prefix_transform: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_pipeline_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_pipeline_tests passed" << std::endl;
    return 0;
}
