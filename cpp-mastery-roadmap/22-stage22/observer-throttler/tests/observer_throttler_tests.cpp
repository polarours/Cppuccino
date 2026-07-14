#include "observer_throttler.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_throttle() {
    observer_throttler::ThrottledDispatcher dispatcher(std::chrono::milliseconds(50));

    int count = 0;
    dispatcher.setHandler([&count](const observer_throttler::Event&) {
        count++;
    });

    dispatcher.emit({"event", "1"});
    dispatcher.emit({"event", "2"});  // Should be throttled

    expect(count == 1, "expected 1 emission");
}

void test_throttle_after_interval() {
    observer_throttler::ThrottledDispatcher dispatcher(std::chrono::milliseconds(10));

    int count = 0;
    dispatcher.setHandler([&count](const observer_throttler::Event&) {
        count++;
    });

    dispatcher.emit({"event", "1"});
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    dispatcher.emit({"event", "2"});

    expect(count == 2, "expected 2 emissions after interval");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Throttler Tests:\n";
        test_basic_throttle();
        std::cout << "  test_basic_throttle: PASS\n";

        test_throttle_after_interval();
        std::cout << "  test_throttle_after_interval: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_throttler_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_throttler_tests passed" << std::endl;
    return 0;
}
