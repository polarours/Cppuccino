#include "abstract_factory.hpp"

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

void test_windows_factory() {
    auto factory = std::make_unique<abstract_factory::WindowsFactory>();
    auto button = factory->createButton();
    auto checkbox = factory->createCheckbox();

    expect(button->getType() == "Windows", "expected Windows button");
    expect(checkbox->getType() == "Windows", "expected Windows checkbox");
}

void test_linux_factory() {
    auto factory = std::make_unique<abstract_factory::LinuxFactory>();
    auto button = factory->createButton();
    auto checkbox = factory->createCheckbox();

    expect(button->getType() == "Linux", "expected Linux button");
    expect(checkbox->getType() == "Linux", "expected Linux checkbox");
}

void test_mac_factory() {
    auto factory = std::make_unique<abstract_factory::MacFactory>();
    auto button = factory->createButton();
    auto checkbox = factory->createCheckbox();

    expect(button->getType() == "Mac", "expected Mac button");
    expect(checkbox->getType() == "Mac", "expected Mac checkbox");
}

void test_factory_consistency() {
    auto factory = std::make_unique<abstract_factory::LinuxFactory>();

    auto button1 = factory->createButton();
    auto button2 = factory->createButton();

    expect(button1->getType() == button2->getType(), "expected consistent types");
}

} // namespace

int main() {
    try {
        std::cout << "Abstract Factory Tests:\n";
        test_windows_factory();
        std::cout << "  test_windows_factory: PASS\n";

        test_linux_factory();
        std::cout << "  test_linux_factory: PASS\n";

        test_mac_factory();
        std::cout << "  test_mac_factory: PASS\n";

        test_factory_consistency();
        std::cout << "  test_factory_consistency: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "abstract_factory_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nabstract_factory_tests passed" << std::endl;
    return 0;
}
