#include "balking.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_balking_on_save_before_open() {
    balking_pattern::Document doc;
    doc.save();
    expect(!doc.isSaved(), "expected not saved before open");
}

void test_save_after_open() {
    balking_pattern::Document doc;
    doc.open();
    doc.save();
    expect(doc.isSaved(), "expected saved after open");
}

void test_balking_on_double_open() {
    balking_pattern::Document doc;
    doc.open();
    doc.open();
    expect(doc.isOpen(), "expected still open");
}

void test_close_balking() {
    balking_pattern::Document doc;
    doc.close();
    expect(!doc.isOpen(), "expected still closed");
}

void test_sensor_threshold() {
    balking_pattern::Sensor sensor(100);

    expect(!sensor.read(50), "expected not triggered");
    expect(!sensor.wasTriggered(), "expected not triggered");

    expect(sensor.read(150), "expected triggered");
    expect(sensor.wasTriggered(), "expected triggered");
    expect(sensor.lastValue() == 150, "expected 150");
}

} // namespace

int main() {
    try {
        std::cout << "Balking Pattern Tests:\n";
        test_balking_on_save_before_open();
        std::cout << "  test_balking_on_save_before_open: PASS\n";

        test_save_after_open();
        std::cout << "  test_save_after_open: PASS\n";

        test_balking_on_double_open();
        std::cout << "  test_balking_on_double_open: PASS\n";

        test_close_balking();
        std::cout << "  test_close_balking: PASS\n";

        test_sensor_threshold();
        std::cout << "  test_sensor_threshold: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "balking_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nbalking_tests passed" << std::endl;
    return 0;
}
