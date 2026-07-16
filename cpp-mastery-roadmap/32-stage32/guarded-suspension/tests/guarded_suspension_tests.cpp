#include "guarded_suspension.hpp"

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

void test_set_and_get() {
    guarded_suspension::GuardedObject obj;
    obj.set(42);
    expect(obj.get() == 42, "expected 42");
    expect(obj.isReady(), "expected ready");
}

void test_wait_for() {
    guarded_suspension::GuardedObject obj;

    std::thread producer([&obj]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        obj.set(100);
    });

    bool result = obj.waitFor([]() { return true; }, std::chrono::milliseconds(100));
    expect(result, "expected wait success");

    producer.join();
    expect(obj.get() == 100, "expected 100");
}

void test_timeout() {
    guarded_suspension::GuardedObject obj;
    bool result = obj.waitFor([]() { return false; }, std::chrono::milliseconds(10));
    expect(!result, "expected timeout");
}

void test_reset() {
    guarded_suspension::GuardedObject obj;
    obj.set(42);
    obj.reset();
    expect(!obj.isReady(), "expected not ready");
    expect(obj.get() == 0, "expected 0 after reset");
}

} // namespace

int main() {
    try {
        std::cout << "Guarded Suspension Tests:\n";
        test_set_and_get();
        std::cout << "  test_set_and_get: PASS\n";

        test_wait_for();
        std::cout << "  test_wait_for: PASS\n";

        test_timeout();
        std::cout << "  test_timeout: PASS\n";

        test_reset();
        std::cout << "  test_reset: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "guarded_suspension_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nguarded_suspension_tests passed" << std::endl;
    return 0;
}
