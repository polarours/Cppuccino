#include "observer_debouncer.hpp"

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_single_emit() {
    observer_debouncer::DebouncedDispatcher dispatcher(std::chrono::milliseconds(10));

    // The handler runs on the dispatcher's own thread, so what it writes
    // has to be read back under the same lock. A sleep is not a lock.
    std::mutex mutex;
    int count = 0;
    std::string received;
    dispatcher.setHandler([&](const observer_debouncer::Event& e) {
        std::lock_guard<std::mutex> lock(mutex);
        count++;
        received = e.data;
    });

    dispatcher.emit({"click", "test"});
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    std::lock_guard<std::mutex> lock(mutex);
    expect(count == 1, "expected 1 emit");
    expect(received == "test", "expected test");
}

void test_rapid_emit_debounce() {
    observer_debouncer::DebouncedDispatcher dispatcher(std::chrono::milliseconds(20));

    std::mutex mutex;
    int count = 0;
    dispatcher.setHandler([&](const observer_debouncer::Event&) {
        std::lock_guard<std::mutex> lock(mutex);
        count++;
    });

    dispatcher.emit({"click", "1"});
    dispatcher.emit({"click", "2"});
    dispatcher.emit({"click", "3"});

    std::this_thread::sleep_for(std::chrono::milliseconds(60));

    int seen;
    {
        std::lock_guard<std::mutex> lock(mutex);
        seen = count;
    }
    expect(dispatcher.emitCount() == 3, "expected 3 emitted (each triggers)");
    expect(seen == 3, "expected the handler to run 3 times");
}

// Regression: the dispatcher must not leave a dispatch running after it is
// gone. The old implementation detached the timer thread, so the handler
// was called on an object that had already been destroyed.
void test_destroy_while_a_dispatch_is_on_its_way() {
    std::mutex mutex;
    int count = 0;

    {
        auto dispatcher = std::make_unique<observer_debouncer::DebouncedDispatcher>(
            std::chrono::milliseconds(200));
        dispatcher->setHandler([&](const observer_debouncer::Event&) {
            std::lock_guard<std::mutex> lock(mutex);
            count++;
        });
        dispatcher->emit({"click", "late"});
        // The dispatch is still waiting for its delay here. Destroying the
        // dispatcher has to cancel it and wait for the thread.
    }

    // Give the delay more than enough time to pass.
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    std::lock_guard<std::mutex> lock(mutex);
    expect(count == 0, "expected the cancelled dispatch not to run");
}

// Regression: the handler was called while the dispatcher still held its
// own mutex. A handler that asks for one more dispatch -- the natural
// thing for a debouncer to do -- deadlocked on a mutex the thread
// already had, and the dispatcher could not even be destroyed afterwards.
void test_handler_may_ask_for_one_more_dispatch() {
    // The dispatcher is on the heap and is not deleted on purpose. If the
    // handler deadlocks, the destructor would wait for the very mutex the
    // stuck thread is holding, and the test would hang instead of failing
    // with a message.
    auto* dispatcher = new observer_debouncer::DebouncedDispatcher(
        std::chrono::milliseconds(20));

    std::atomic<int> seen{0};
    dispatcher->setHandler([&](const observer_debouncer::Event& event) {
        seen++;
        if (event.type == "first") {
            dispatcher->emit({"second", "again"});
        }
    });

    dispatcher->emit({"first", "once"});

    // If the handler deadlocks, the second event never arrives.
    for (int i = 0; i < 500 && seen.load() < 2; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    expect(seen.load() == 2,
           "expected both events to be delivered, got " +
               std::to_string(seen.load()));

    delete dispatcher;
}

} // namespace

int main() {
    try {
        std::cout << "Observer Debouncer Tests:\n";
        test_single_emit();
        std::cout << "  test_single_emit: PASS\n";

        test_rapid_emit_debounce();
        std::cout << "  test_rapid_emit_debounce: PASS\n";

        test_destroy_while_a_dispatch_is_on_its_way();
        std::cout << "  test_destroy_while_a_dispatch_is_on_its_way: PASS\n";

        test_handler_may_ask_for_one_more_dispatch();
        std::cout << "  test_handler_may_ask_for_one_more_dispatch: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "observer_debouncer_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nobserver_debouncer_tests passed" << std::endl;
    return 0;
}
