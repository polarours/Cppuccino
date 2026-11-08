#include "guarded_suspension.hpp"

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <string>
#include <chrono>
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

// Regression: the predicate of waitFor() is called while the lock is
// held, because that is what a condition variable asks for. The state
// was behind the same mutex, so a predicate that asked the object about
// itself -- waitFor([&] { return obj.isReady(); }) -- waited for a lock
// the thread already had, and never came back.
void test_predicate_may_ask_the_object() {
    guarded_suspension::GuardedObject obj;

    std::thread producer([&obj]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        obj.set(42);
    });

    std::atomic<bool> finished{false};
    std::atomic<bool> ready{false};

    // The wait runs on its own thread: if the predicate deadlocks, the
    // thread that waits is stuck with it, and the test has to be able to
    // say so instead of hanging.
    std::thread waiter([&]() {
        ready = obj.waitFor([&obj]() { return obj.isReady(); },
                            std::chrono::milliseconds(500));
        finished = true;
    });

    for (int i = 0; i < 500 && !finished.load(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (!finished.load()) {
        waiter.detach();
        producer.detach();
        expect(false, "the predicate deadlocked when it asked the object "
                      "about itself");
    }
    waiter.join();
    producer.join();

    expect(ready.load(), "expected the object to be ready");
    expect(obj.get() == 42, "expected 42");
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

        test_predicate_may_ask_the_object();
        std::cout << "  test_predicate_may_ask_the_object: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "guarded_suspension_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nguarded_suspension_tests passed" << std::endl;
    return 0;
}
