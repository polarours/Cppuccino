#include "observer_pipeline.hpp"

#include <iostream>

int main() {
    std::cout << "=== Observer Pipeline Demo ===\n\n";

    observer_pipeline::Pipeline<std::string> pipeline;
    pipeline.addStage(observer_pipeline::trim);
    pipeline.addStage(observer_pipeline::toUpper);

    std::string input = "  hello world  ";
    std::string result = pipeline.process(input);

    std::cout << "Input: \"" << input << "\"\n";
    std::cout << "Output: \"" << result << "\"\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
