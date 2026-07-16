#include "double_buffer.hpp"

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

void test_write_read_swap() {
    double_buffer::DoubleBuffer<int> buf(5);
    std::vector<int> data = {1, 2, 3, 4, 5};

    buf.write(data);
    buf.swap();

    expect(buf.getFront(0) == 1, "expected 1");
    expect(buf.getFront(4) == 5, "expected 5");
}

void test_read_before_swap() {
    double_buffer::DoubleBuffer<int> buf(3);
    std::vector<int> data = {10, 20, 30};

    buf.write(data);
    auto before = buf.read();
    expect(before[0] == 0, "expected 0 before swap (default init)");
}

void test_concurrent_write_read() {
    double_buffer::DoubleBuffer<int> buf(100);
    std::vector<int> data(100, 42);

    std::thread writer([&buf, &data]() {
        buf.write(data);
        buf.swap();
    });

    auto result = buf.read();
    writer.join();

    expect(result.size() == 100, "expected 100");
}

void test_set_back() {
    double_buffer::DoubleBuffer<int> buf(3);
    buf.setBack(0, 10);
    buf.setBack(1, 20);
    buf.setBack(2, 30);
    buf.swap();

    expect(buf.getFront(0) == 10, "expected 10");
    expect(buf.getFront(1) == 20, "expected 20");
    expect(buf.getFront(2) == 30, "expected 30");
}

} // namespace

int main() {
    try {
        std::cout << "Double Buffer Tests:\n";
        test_write_read_swap();
        std::cout << "  test_write_read_swap: PASS\n";

        test_read_before_swap();
        std::cout << "  test_read_before_swap: PASS\n";

        test_concurrent_write_read();
        std::cout << "  test_concurrent_write_read: PASS\n";

        test_set_back();
        std::cout << "  test_set_back: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "double_buffer_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ndouble_buffer_tests passed" << std::endl;
    return 0;
}
