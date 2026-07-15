#include "mvc_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_add_and_render() {
    mvc_pattern::TodoController controller;
    controller.addItem("Buy milk");
    controller.addItem("Walk dog");
    controller.refresh();

    expect(controller.model().size() == 2, "expected 2 items");
    expect(controller.output().size() == 3, "expected 2 items + 1 summary");
}

void test_toggle() {
    mvc_pattern::TodoController controller;
    controller.addItem("Task 1");
    controller.toggleItem(1);
    controller.refresh();

    expect(controller.model().doneCount() == 1, "expected 1 done");
    expect(controller.output()[0].find("[x]") != std::string::npos, "expected done marker");
}

void test_remove() {
    mvc_pattern::TodoController controller;
    controller.addItem("Task 1");
    controller.addItem("Task 2");
    controller.removeItem(1);
    controller.refresh();

    expect(controller.model().size() == 1, "expected 1 item after remove");
}

void test_summary() {
    mvc_pattern::TodoController controller;
    controller.addItem("Task 1");
    controller.addItem("Task 2");
    controller.toggleItem(1);
    controller.refresh();

    auto& output = controller.output();
    expect(output.back().find("Total: 2") != std::string::npos, "expected total 2");
    expect(output.back().find("Done: 1") != std::string::npos, "expected done 1");
}

} // namespace

int main() {
    try {
        std::cout << "MVC Pattern Tests:\n";
        test_add_and_render();
        std::cout << "  test_add_and_render: PASS\n";

        test_toggle();
        std::cout << "  test_toggle: PASS\n";

        test_remove();
        std::cout << "  test_remove: PASS\n";

        test_summary();
        std::cout << "  test_summary: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "mvc_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nmvc_pattern_tests passed" << std::endl;
    return 0;
}
