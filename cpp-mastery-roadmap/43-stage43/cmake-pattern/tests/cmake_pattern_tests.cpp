#include "cmake_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_project_basics() {
    cmake_pattern::CMakeProject project;
    project.setName("MyApp");
    project.setVersion("2.0.0");
    project.setCxxStandard(20);

    expect(project.name() == "MyApp", "expected MyApp");
    expect(project.version() == "2.0.0", "expected 2.0.0");
    expect(project.cxxStandard() == 20, "expected C++20");
}

void test_add_library() {
    cmake_pattern::CMakeProject project;
    project.setName("MyLib");
    project.addLibrary("mylib");

    expect(project.libraryCount() == 1, "expected 1 library");
}

void test_add_test() {
    cmake_pattern::CMakeProject project;
    project.setName("MyLib");
    project.addTest("mylib_tests");

    expect(project.testCount() == 1, "expected 1 test");
}

void test_generate_output() {
    cmake_pattern::CMakeProject project;
    project.setName("TestProject");
    project.setVersion("1.0.0");
    project.setCxxStandard(17);
    project.addOption("BUILD_TESTING", "Enable tests", true);
    project.addLibrary("mylib");
    project.addTest("mylib_tests");

    auto output = project.generate();
    expect(output.find("project(TestProject") != std::string::npos, "expected project name");
    expect(output.find("CMAKE_CXX_STANDARD 17") != std::string::npos, "expected C++17");
    expect(output.find("option(BUILD_TESTING") != std::string::npos, "expected BUILD_TESTING");
    expect(output.find("add_library(mylib") != std::string::npos, "expected library");
    expect(output.find("add_test") != std::string::npos, "expected add_test");
}

void test_multiple_options() {
    cmake_pattern::CMakeProject project;
    project.setName("Multi");
    project.addOption("DEBUG", "Enable debug", false);
    project.addOption("PROFILE", "Enable profiling", false);

    expect(project.optionCount() == 2, "expected 2 options");
}

} // namespace

int main() {
    try {
        std::cout << "CMake Pattern Tests:\n";
        test_project_basics();
        std::cout << "  test_project_basics: PASS\n";

        test_add_library();
        std::cout << "  test_add_library: PASS\n";

        test_add_test();
        std::cout << "  test_add_test: PASS\n";

        test_generate_output();
        std::cout << "  test_generate_output: PASS\n";

        test_multiple_options();
        std::cout << "  test_multiple_options: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "cmake_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncmake_pattern_tests passed" << std::endl;
    return 0;
}
