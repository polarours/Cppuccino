#include "cmake_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== CMake Pattern Demo ===\n\n";

    cmake_pattern::CMakeProject project;
    project.setName("DemoProject");
    project.setVersion("1.0.0");
    project.setCxxStandard(17);
    project.addOption("BUILD_TESTING", "Enable tests", true);
    project.addLibrary("mylib");
    project.addTest("mylib_tests");

    std::cout << project.generate();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
