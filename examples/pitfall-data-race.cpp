// examples/pitfall-data-race.cpp
// Demonstrates data races and the std::atomic / std::mutex fixes.
// Compile with: g++ -std=c++17 -o pitfall_data_race pitfall-data-race.cpp

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

constexpr int kThreads = 4;
constexpr int kIterations = 100'000;

// --- 1. The broken version: a plain shared counter ------------------------

int g_racy_counter = 0;

void racy_worker() {
    for (int i = 0; i < kIterations; ++i) {
        // BAD: load, add and store are three separate steps, so two threads
        // can read the same value and both write back the same increment.
        ++g_racy_counter;
    }
}

// The same function, made well-defined with an atomic.
std::atomic<int> g_atomic_counter{0};

void atomic_worker() {
    for (int i = 0; i < kIterations; ++i) {
        ++g_atomic_counter;  // atomic read-modify-write
    }
}

// --- 2. A mutex for a compound invariant ----------------------------------

struct Account {
    int balance;
};

std::mutex g_bank_mutex;
Account g_savings{1000};
Account g_checking{500};

void bad_transfer() {
    // BAD: the two writes are not atomic with respect to each other, so a
    // reader can observe money that is in neither account.
    g_savings.balance -= 100;
    g_checking.balance += 100;
}

void good_transfer() {
    // GOOD: the whole transfer happens under the lock
    std::lock_guard<std::mutex> lock(g_bank_mutex);
    g_savings.balance -= 100;
    g_checking.balance += 100;
}

// --- 3. Runners -----------------------------------------------------------

template <typename Body>
void run_under_contention(Body body) {
    std::vector<std::thread> workers;
    for (int i = 0; i < kThreads; ++i) {
        workers.emplace_back(body, i);
    }
    for (auto& worker : workers) {
        worker.join();
    }
}

void broken_data_race() {
    std::cout << "--- Broken: unsynchronized ++counter ---\n";

    // Two threads may read the same value and both write back the same
    // increment, so the total is smaller than it should be.  Worse, the race
    // is undefined behaviour: the compiler may hoist the counter into a
    // register, and the result is not merely "sometimes wrong" but undefined.
    //
    // run_under_contention([](int) { racy_worker(); });

    std::cout << "Expected total: " << kThreads * kIterations << "\n";
    std::cout << "The racy run is left commented out: a data race is\n"
                 "undefined behaviour, so the printed value would prove\n"
                 "nothing beyond the schedule of that one run.\n";
}

void lost_update_by_interleaving() {
    std::cout << "\n--- Why the increment is lost ---\n";

    // The same interleaving, written out step by step so it is visible
    // every time the program runs.
    int counter = 5;

    std::cout << "counter starts at " << counter << "\n";
    std::cout << "thread A reads " << counter << "\n";
    std::cout << "thread B reads " << counter << "\n";
    std::cout << "thread A writes " << counter + 1 << "\n";
    std::cout << "thread B writes " << counter + 1 << "  <- A's increment is gone\n";
    std::cout << "counter is now " << counter + 1 << " instead of " << counter + 2 << "\n";
}

void fixed_with_atomic() {
    std::cout << "\n--- Fixed: std::atomic counter ---\n";

    run_under_contention([](int) { atomic_worker(); });

    std::cout << "counter: " << g_atomic_counter.load() << "\n";
    std::cout << "expected: " << kThreads * kIterations << "\n";
    std::cout << "exact match: "
              << (g_atomic_counter.load() == kThreads * kIterations ? "yes" : "no") << "\n";
}

void fixed_with_mutex() {
    std::cout << "\n--- Fixed: std::mutex for a compound invariant ---\n";

    std::cout << "before: savings=" << g_savings.balance
              << " checking=" << g_checking.balance << "\n";

    run_under_contention([](int) { good_transfer(); });

    std::cout << "after:  savings=" << g_savings.balance
              << " checking=" << g_checking.balance << "\n";
    std::cout << "total preserved: "
              << ((g_savings.balance + g_checking.balance == 1500) ? "yes" : "no") << "\n";
}

// --- 4. The broken transfer, described but not run ------------------------

void broken_transfer() {
    std::cout << "\n--- Broken: transfer without a lock ---\n";

    // BAD: the two balances are updated one after the other, so a reader
    // that runs in between sees savings already debited but checking not
    // yet credited: 100 of the 1500 units exist in neither account.
    //
    // run_under_contention([](int) { bad_transfer(); });

    // The same code under the mutex is what good_transfer() does.
    std::cout << "The unlocked transfer is left commented out: it is a data\n"
                 "race on two unrelated variables, and the intermediate state\n"
                 "is only observable from another thread.\n";
}

// --- 5. How the bug is found ----------------------------------------------

void detection() {
    std::cout << "\n--- Detection ---\n";
    std::cout << "g++ -fsanitize=thread -g -O1 main.cpp -o main\n";
    std::cout << "ThreadSanitizer reports the two racing accesses with\n"
                 "their stack traces, which no amount of rerunning finds.\n";
}

int main() {
    broken_data_race();
    lost_update_by_interleaving();
    fixed_with_atomic();
    broken_transfer();
    fixed_with_mutex();
    detection();

    std::cout << "\n=== All data race scenarios completed ===\n";
    return 0;
}
