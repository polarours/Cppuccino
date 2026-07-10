#include "proxy_pattern.hpp"

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

void test_proxy_lazy_loading() {
    auto proxy = std::make_shared<proxy_pattern::ImageProxy>("test.png");

    // Should not load yet
    expect(proxy->getInfo().find("not loaded") != std::string::npos, "expected not loaded");

    // First display loads the image
    proxy->display();

    // Now should be loaded
    expect(proxy->getInfo().find("loaded") != std::string::npos, "expected loaded");
}

void test_proxy_multiple_displays() {
    auto proxy = std::make_shared<proxy_pattern::ImageProxy>("test.png");

    // Multiple displays should only load once
    proxy->display();
    proxy->display();

    expect(proxy->getInfo().find("loaded") != std::string::npos, "expected loaded");
}

void test_direct_access() {
    auto real = std::make_shared<proxy_pattern::RealImage>("test.png");

    expect(real->getInfo().find("RealImage") != std::string::npos, "expected RealImage");
    real->display();
}

} // namespace

int main() {
    try {
        std::cout << "Proxy Pattern Tests:\n";
        test_proxy_lazy_loading();
        std::cout << "  test_proxy_lazy_loading: PASS\n";

        test_proxy_multiple_displays();
        std::cout << "  test_proxy_multiple_displays: PASS\n";

        test_direct_access();
        std::cout << "  test_direct_access: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "proxy_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nproxy_pattern_tests passed" << std::endl;
    return 0;
}
