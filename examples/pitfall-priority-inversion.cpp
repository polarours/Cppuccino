// examples/pitfall-priority-inversion.cpp
// Demonstrates priority inversion and its fixes: priority inheritance, short critical sections and lock-free design.
// Compile with: g++ -std=c++17 -o pitfall_priority_inversion pitfall-priority-inversion.cpp

#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

// --- A deterministic model of a strict priority scheduler ---

enum class Priority { Low = 0, Medium = 1, High = 2 };

struct Task {
    std::string name;
    Priority effective = Priority::Low;
    int runnable_at = 1;  // the tick at which the task becomes runnable
    int work_left = 0;    // units of work still to do
    bool blocked = false;  // waiting for the shared resource
    bool holds_resource = false;
    bool finished = false;
};

struct Scenario {
    bool priority_inheritance = false;
    bool high_needs_resource = true;
    int low_critical_work = 10;  // work done while holding the resource
    int medium_work = 100;       // work done by the interfering task
};

struct Result {
    int ticks = 0;
    int high_wait_ticks = 0;
};

// Timeline of the modelled system:
//   tick 1: Low takes the resource and enters its critical section
//   tick 2: High arrives, the resource is taken, so High blocks
//   tick 3: Medium arrives, it does not need the resource and preempts Low
Result simulate(const Scenario& scenario) {
    Task low{"Low", Priority::Low, 1, scenario.low_critical_work};
    Task medium{"Medium", Priority::Medium, 3, scenario.medium_work};
    Task high{"High", Priority::High, 2, 1};

    bool resource_taken = false;
    bool high_started = false;
    int high_wait_ticks = 0;
    int tick = 0;

    while (!high.finished && tick < 100000) {
        ++tick;

        // Pick the runnable task with the highest effective priority.
        Task* chosen = nullptr;
        for (Task* candidate : {&low, &medium, &high}) {
            if (candidate->finished || candidate->blocked) {
                continue;
            }
            if (candidate->runnable_at > tick) {
                continue;
            }
            if (chosen == nullptr || candidate->effective > chosen->effective) {
                chosen = candidate;
            }
        }
        if (chosen == nullptr) {
            break;
        }

        if (chosen == &low) {
            if (!low.holds_resource) {
                low.holds_resource = true;
                resource_taken = true;
            }
            if (low.work_left > 0) {
                --low.work_left;
            }
            if (low.work_left == 0) {
                // The critical section is over: the resource is released.
                low.holds_resource = false;
                resource_taken = false;
                if (high.blocked) {
                    high.blocked = false;          // the waiter wakes up
                    low.effective = Priority::Low;  // the inherited priority is dropped
                }
            }
        } else if (chosen == &medium) {
            if (medium.work_left > 0) {
                --medium.work_left;
            }
            if (medium.work_left == 0) {
                medium.finished = true;
            }
        } else {
            if (!high_started) {
                high_started = true;
                if (scenario.high_needs_resource) {
                    if (resource_taken) {
                        high.blocked = true;
                        if (scenario.priority_inheritance) {
                            // Priority inheritance: the lock holder borrows the
                            // priority of the task that is waiting for it.
                            low.effective = Priority::High;
                        }
                    } else {
                        high.holds_resource = true;
                    }
                }
            }
            if (!high.blocked) {
                if (high.work_left > 0) {
                    --high.work_left;
                }
                if (high.work_left == 0) {
                    high.finished = true;
                    high.holds_resource = false;
                }
            }
        }

        if (high.blocked) {
            ++high_wait_ticks;
        }
    }

    return Result{tick, high_wait_ticks};
}

void report(const char* title, const Scenario& scenario) {
    const Result result = simulate(scenario);
    std::cout << "--- " << title << " ---\n";
    std::cout << "  High blocked for " << result.high_wait_ticks
              << " ticks, the whole run took " << result.ticks << " ticks\n";
}

void modelledScenarios() {
    std::cout << "--- Modelled strict priority scheduler ---\n";

    report("No inheritance, long critical section: the pitfall",
           Scenario{false, true, 10, 100});

    report("Priority inheritance: the fix",
           Scenario{true, true, 10, 100});

    report("The high-priority task does not need the shared resource",
           Scenario{false, false, 10, 100});

    report("No medium interference, long critical section",
           Scenario{false, true, 10, 0});

    report("No medium interference, short critical section",
           Scenario{false, true, 1, 0});
}

// --- The same blocking, measured with real threads ---

void realThreadBlocking() {
    std::cout << "\n--- Real threads: a high-priority thread blocked on a mutex ---\n";

    std::mutex resource;

    std::thread low([&resource] {
        std::lock_guard<std::mutex> guard(resource);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));  // holds the lock
    });

    std::thread high([&resource] {
        const auto start = std::chrono::steady_clock::now();
        std::lock_guard<std::mutex> guard(resource);
        const auto waited = std::chrono::steady_clock::now() - start;
        std::cout << "  the high-priority thread waited "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(waited).count()
                  << " ms for the resource\n";
    });

    high.join();
    low.join();
}

int main() {
    modelledScenarios();
    realThreadBlocking();

    std::cout << "\n=== All priority inversion scenarios completed ===\n";
    return 0;
}
