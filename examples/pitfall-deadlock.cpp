// examples/pitfall-deadlock.cpp
// Demonstrates a lock-ordering deadlock and the deadlock-free std::scoped_lock fix.
// Compile with: g++ -std=c++17 -o pitfall_deadlock pitfall-deadlock.cpp

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// A reusable rendezvous point: the call returns once `count` threads arrived.
class Barrier {
public:
    explicit Barrier(int count) : threshold_(count), arrived_(0), generation_(0) {}

    void arrive_and_wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        const int generation = generation_;
        if (++arrived_ == threshold_) {
            arrived_ = 0;
            ++generation_;
            lock.unlock();
            condition_.notify_all();
            return;
        }
        condition_.wait(lock, [this, generation] { return generation_ != generation; });
    }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    int threshold_;
    int arrived_;
    int generation_;
};

// --- 1. The deadlock pattern ------------------------------------------------
//
// std::mutex mtx_a;
// std::mutex mtx_b;
//
// void thread_one() {
//     std::lock_guard<std::mutex> la(mtx_a);
//     std::lock_guard<std::mutex> lb(mtx_b);  // waits for mtx_b
// }
//
// void thread_two() {
//     std::lock_guard<std::mutex> lb(mtx_b);  // reversed order
//     std::lock_guard<std::mutex> la(mtx_a);  // waits for mtx_a
// }
//
// If thread_one owns mtx_a and thread_two owns mtx_b, both wait for the other
// forever: a circular wait.  The program hangs, so the pattern is shown, not
// run.

std::timed_mutex demo_a;
std::timed_mutex demo_b;

void deadlock_pattern() {
    std::cout << "--- 1. The deadlock pattern (observed, never entered) ---\n";

    Barrier both_holding(2);
    Barrier both_tried(2);
    std::atomic<bool> one_got_b{false};
    std::atomic<bool> two_got_a{false};

    auto thread_one = [&] {
        std::lock_guard<std::timed_mutex> owns_a(demo_a);
        both_holding.arrive_and_wait();  // now both threads hold one mutex
        // BAD: a blocking lock() here would wait forever, because thread two
        // holds mtx_b and waits for mtx_a.  try_lock_for lets the program
        // observe the deadlock instead of hanging on it.
        if (demo_b.try_lock_for(std::chrono::milliseconds(20))) {
            one_got_b.store(true);
            demo_b.unlock();
        }
        both_tried.arrive_and_wait();  // neither thread gives up its mutex early
    };

    auto thread_two = [&] {
        std::lock_guard<std::timed_mutex> owns_b(demo_b);
        both_holding.arrive_and_wait();
        if (demo_a.try_lock_for(std::chrono::milliseconds(20))) {
            two_got_a.store(true);
            demo_a.unlock();
        }
        both_tried.arrive_and_wait();
    };

    std::thread one(thread_one);
    std::thread two(thread_two);
    one.join();
    two.join();

    std::cout << "thread one holds mtx_a and could not take mtx_b: "
              << (!one_got_b.load() ? "yes" : "no") << "\n";
    std::cout << "thread two holds mtx_b and could not take mtx_a: "
              << (!two_got_a.load() ? "yes" : "no") << "\n";
    std::cout << "both threads are blocked on each other -> deadlock\n";
}

// --- 2. The fix: std::scoped_lock -------------------------------------------

std::mutex fix_a;
std::mutex fix_b;

void scoped_lock_fix() {
    std::cout << "\n--- 2. The fix: std::scoped_lock ---\n";

    Barrier start(2);
    std::atomic<int> completed{0};

    auto thread_one = [&] {
        start.arrive_and_wait();
        // The argument order is reversed compared to thread two, but
        // std::scoped_lock acquires both mutexes without deadlocking.
        std::scoped_lock lock(fix_a, fix_b);
        ++completed;
    };

    auto thread_two = [&] {
        start.arrive_and_wait();
        std::scoped_lock lock(fix_b, fix_a);
        ++completed;
    };

    std::thread one(thread_one);
    std::thread two(thread_two);
    one.join();
    two.join();

    std::cout << "threads that finished: " << completed.load() << " (expected 2)\n";
    std::cout << "std::scoped_lock breaks the cycle by construction\n";
}

// --- 3. The other fix: one global lock order --------------------------------

std::mutex order_a;
std::mutex order_b;

void transfer_in_global_order(std::mutex& first, std::mutex& second, int& counter) {
    // Rule: always take the mutex with the lower address first.  Both threads
    // therefore acquire the two mutexes in the same relative order, so no
    // cycle can form.
    //
    // std::scoped_lock is the modern equivalent of this rule; the explicit
    // lock/unlock is shown here to make the ordering visible.
    std::mutex* lower = (&first < &second) ? &first : &second;
    std::mutex* higher = (&first < &second) ? &second : &first;
    lower->lock();
    higher->lock();
    ++counter;
    higher->unlock();
    lower->unlock();
}

void global_order_fix() {
    std::cout << "\n--- 3. The other fix: one global lock order ---\n";

    Barrier start(2);
    int counter = 0;

    auto thread_one = [&] {
        start.arrive_and_wait();
        transfer_in_global_order(order_a, order_b, counter);
    };

    auto thread_two = [&] {
        start.arrive_and_wait();
        transfer_in_global_order(order_b, order_a, counter);  // reversed call
    };

    std::thread one(thread_one);
    std::thread two(thread_two);
    one.join();
    two.join();

    std::cout << "transfers completed: " << counter << " (expected 2)\n";
    std::cout << "sorting the mutexes by address gives every thread the\n"
                 "same order, so the wait-for graph stays acyclic\n";
}

// --- 4. How the bug is found ------------------------------------------------

void detection() {
    std::cout << "\n--- 4. Detection ---\n";
    std::cout << "g++ -fsanitize=thread -g -O1 main.cpp -o main\n";
    std::cout << "ThreadSanitizer reports the lock-order inversion between\n"
                 "the two mutexes, which no amount of rerunning reveals.\n";
}

int main() {
    deadlock_pattern();
    scoped_lock_fix();
    global_order_fix();
    detection();

    std::cout << "\n=== All deadlock scenarios completed ===\n";
    return 0;
}
