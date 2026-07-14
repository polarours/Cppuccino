#include "observer_debouncer.hpp"

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

void test_single_emit() {
    observer_debouncer::DebouncedDispatcher dispatcher(std::chrono::milliseconds(10));

    int count = 0;
    std::string received;
    dispatcher.setHandler([&](const observer_debouncer::Event& e) {
        count++;
        received = e.data;
    });

    dispatcher.emit({"click", "test"});
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    expect(count == 1, "expected 1 emit");
    expect(received == "test", "expected test");
}

void test_rapid_emit_debounce() {
    observer_debouncer::DebouncedDispatcher dispatcher(std::chrono::milliseconds(20));

    int count = 0;
    dispatcher.setHandler([&](const observer_debouncer::Event&) {
        count++;
    });

    dispatcher.emit({"click", "1"});
    dispatcher.emit({"click", "2"});
    dispatcher.emit({"click", "3"});

    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    expect(dispatcher.emitCount() == 3, "expected 3 emitted (each triggers)");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Debouncer Tests:\n";
        test_single_emit();
        std::cout << "  test_single_emit: PASS\n";

        test_rapid_emit_debounce();
        std::cout << "  test_rapid_emit_debounce: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_debouncer_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_debouncer_tests passed" << std::endl;
    return 0;
}
