#include "strategy_pipeline.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Strategy Pipeline Demo ===\n\n";

    strategy_pipeline::Pipeline pipeline;
    pipeline.add(std::make_shared<strategy_pipeline::TrimTransform>());
    pipeline.add(std::make_shared<strategy_pipeline::UpperTransform>());
    pipeline.add(std::make_shared<strategy_pipeline::PrefixTransform>("[OUT] "));

    std::cout << pipeline.execute("  hello world  ") << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
