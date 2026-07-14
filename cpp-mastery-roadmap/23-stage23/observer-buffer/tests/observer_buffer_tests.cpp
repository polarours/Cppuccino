#include "observer_buffer.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_buffer_and_flush() {
    observer_buffer::BufferedDispatcher dispatcher(3);

    int count = 0;
    dispatcher.setHandler([&count](const observer_buffer::Event&) {
        count++;
    });

    dispatcher.emit({"event", "1"});
    dispatcher.emit({"event", "2"});
    expect(count == 0, "expected 0 before flush");

    dispatcher.flush();
    expect(count == 2, "expected 2 after flush");
}

void test_auto_flush() {
    observer_buffer::BufferedDispatcher dispatcher(2);

    int count = 0;
    dispatcher.setHandler([&count](const observer_buffer::Event&) {
        count++;
    });

    dispatcher.emit({"event", "1"});
    dispatcher.emit({"event", "2"});
    expect(count == 2, "expected auto flush at buffer size");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Buffer Tests:\n";
        test_buffer_and_flush();
        std::cout << "  test_buffer_and_flush: PASS\n";

        test_auto_flush();
        std::cout << "  test_auto_flush: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_buffer_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_buffer_tests passed" << std::endl;
    return 0;
}
