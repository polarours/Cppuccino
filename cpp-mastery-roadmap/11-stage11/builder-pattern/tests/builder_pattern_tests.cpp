#include "builder_pattern.hpp"

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

void test_gaming_computer() {
    auto builder = std::make_shared<builder_pattern::GamingComputerBuilder>();
    builder->buildCPU();
    builder->buildRAM();
    builder->buildStorage();
    builder->buildGPU();

    auto computer = builder->getResult();
    expect(computer->getCPU() == "Intel i9-12900K", "expected i9 CPU");
    expect(computer->getGPU() == "NVIDIA RTX 4090", "expected RTX 4090");
}

void test_office_computer() {
    auto builder = std::make_shared<builder_pattern::OfficeComputerBuilder>();
    builder->buildCPU();
    builder->buildRAM();
    builder->buildStorage();
    builder->buildGPU();

    auto computer = builder->getResult();
    expect(computer->getCPU() == "Intel i5-12400", "expected i5 CPU");
    expect(computer->getGPU() == "Integrated Graphics", "expected integrated GPU");
}

void test_director() {
    builder_pattern::ComputerDirector director;

    director.setBuilder(std::make_shared<builder_pattern::WorkstationBuilder>());
    auto computer = director.construct();

    expect(computer->getCPU() == "AMD Threadripper", "expected Threadripper");
    expect(computer->getRAM() == "64GB DDR5", "expected 64GB RAM");
}

void test_builder_reuse() {
    auto builder = std::make_shared<builder_pattern::GamingComputerBuilder>();

    builder->buildCPU();
    auto computer1 = builder->getResult();

    builder->buildRAM();
    auto computer2 = builder->getResult();

    // Both should share the same underlying computer
    expect(computer1.get() == computer2.get(), "expected same pointer");
}

} // namespace

int main() {
    try {
        std::cout << "Builder Pattern Tests:\n";
        test_gaming_computer();
        std::cout << "  test_gaming_computer: PASS\n";

        test_office_computer();
        std::cout << "  test_office_computer: PASS\n";

        test_director();
        std::cout << "  test_director: PASS\n";

        test_builder_reuse();
        std::cout << "  test_builder_reuse: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "builder_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nbuilder_pattern_tests passed" << std::endl;
    return 0;
}
