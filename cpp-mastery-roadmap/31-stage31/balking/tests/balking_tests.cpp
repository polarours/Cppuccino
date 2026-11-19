#include "balking.hpp"

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

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

// Regression: isOpen()/isSaved() read plain bools that another thread wrote
// under the mutex, which was a data race. The state is atomic now.
void test_concurrent_reader() {
    balking_pattern::Document doc;
    doc.open();

    std::atomic<int> openReads{0};
    std::thread writer([&doc] {
        for (int i = 0; i < 1000; ++i) {
            doc.close();
            doc.open();
        }
    });
    for (int i = 0; i < 1000; ++i) {
        if (doc.isOpen()) ++openReads;
        (void)doc.isSaved();
    }
    writer.join();

    expect(openReads.load() > 0, "expected the reader to see the document open");
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

        test_concurrent_reader();
        std::cout << "  test_concurrent_reader: PASS\n";

        test_sensor_threshold();
        std::cout << "  test_sensor_threshold: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "balking_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nbalking_tests passed" << std::endl;
    return 0;
}
