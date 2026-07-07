#include "event_loop.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <atomic>
#include <chrono>
#include <thread>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_post() {
    event_loop::EventLoop loop;
    std::atomic<bool> executed{false};

    loop.post([&]() {
        executed = true;
        loop.stop();
    });

    loop.run();
    expect(executed, "expected task to execute");
}

void test_multiple_posts() {
    event_loop::EventLoop loop;
    std::atomic<int> counter{0};

    for (int i = 0; i < 5; ++i) {
        loop.post([&counter, &loop, i]() {
            counter++;
            if (i == 4) {
                loop.stop();
            }
        });
    }

    loop.run();
    expect(counter == 5, "expected 5 tasks executed");
}

void test_schedule() {
    event_loop::EventLoop loop;
    std::atomic<bool> executed{false};

    loop.schedule([&]() {
        executed = true;
        loop.stop();
    }, std::chrono::milliseconds(10));

    loop.run();
    expect(executed, "expected scheduled task to execute");
}

void test_schedule_repeating() {
    event_loop::EventLoop loop;
    std::atomic<int> counter{0};

    event_loop::TimerId id = 0;
    id = loop.scheduleRepeating([&]() {
        counter++;
        if (counter >= 3) {
            loop.cancelTimer(id);
            loop.stop();
        }
    }, std::chrono::milliseconds(5));

    loop.run();
    expect(counter >= 3, "expected at least 3 executions");
}

void test_cancel_timer() {
    event_loop::EventLoop loop;
    std::atomic<bool> executed{false};

    auto id = loop.schedule([&]() {
        executed = true;
    }, std::chrono::milliseconds(10));

    loop.cancelTimer(id);

    loop.schedule([&]() {
        loop.stop();
    }, std::chrono::milliseconds(50));

    loop.run();
    expect(!executed, "expected cancelled timer not to execute");
}

void test_stop() {
    event_loop::EventLoop loop;
    std::atomic<int> counter{0};

    loop.post([&counter, &loop]() {
        counter++;
        loop.stop();
    });

    // This task should not execute because stop is called in the first task
    loop.post([&counter]() {
        counter++;
    });

    loop.run();
    // Only the first task should have executed because stop() is called inside it
    expect(counter == 1, "expected only first task after stop");
}

void test_is_running() {
    event_loop::EventLoop loop;
    expect(!loop.isRunning(), "expected not running initially");

    loop.post([&]() {
        expect(loop.isRunning(), "expected running during execution");
        loop.stop();
    });

    loop.run();
    expect(!loop.isRunning(), "expected not running after stop");
}

void test_has_pending_tasks() {
    event_loop::EventLoop loop;
    expect(!loop.hasPendingTasks(), "expected no pending tasks");

    loop.post([&]() {});
    expect(loop.hasPendingTasks(), "expected pending tasks after post");
}

} // namespace

int main() {
    try {
        std::cout << "Event Loop Tests:\n";
        test_basic_post();
        std::cout << "  test_basic_post: PASS\n";

        test_multiple_posts();
        std::cout << "  test_multiple_posts: PASS\n";

        test_schedule();
        std::cout << "  test_schedule: PASS\n";

        test_schedule_repeating();
        std::cout << "  test_schedule_repeating: PASS\n";

        test_cancel_timer();
        std::cout << "  test_cancel_timer: PASS\n";

        test_stop();
        std::cout << "  test_stop: PASS\n";

        test_is_running();
        std::cout << "  test_is_running: PASS\n";

        test_has_pending_tasks();
        std::cout << "  test_has_pending_tasks: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "event_loop_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nevent_loop_tests passed" << std::endl;
    return 0;
}
