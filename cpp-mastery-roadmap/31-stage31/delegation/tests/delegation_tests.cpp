#include "delegation.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_button_delegates() {
    delegation_pattern::Button btn(std::make_shared<delegation_pattern::TextRenderer>("Click"), "OK");
    expect(btn.draw() == "[OK: Click]", "expected [OK: Click]");
}

void test_textfield_delegates() {
    delegation_pattern::TextField tf(std::make_shared<delegation_pattern::TextRenderer>("Hello"));
    expect(tf.draw() == "(Hello)", "expected (Hello)");
}

void test_change_renderer() {
    delegation_pattern::Button btn(std::make_shared<delegation_pattern::TextRenderer>("Click"), "OK");
    expect(btn.draw() == "[OK: Click]", "expected initial");

    btn.setRenderer(std::make_shared<delegation_pattern::TextRenderer>("Submit"));
    expect(btn.draw() == "[OK: Submit]", "expected [OK: Submit]");
}

} // namespace

int main() {
    try {
        std::cout << "Delegation Pattern Tests:\n";
        test_button_delegates();
        std::cout << "  test_button_delegates: PASS\n";

        test_textfield_delegates();
        std::cout << "  test_textfield_delegates: PASS\n";

        test_change_renderer();
        std::cout << "  test_change_renderer: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "delegation_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ndelegation_tests passed" << std::endl;
    return 0;
}
