// examples/pitfall-livelock.cpp
// Demonstrates a polite-yield livelock and the fixes: random backoff and a single owner.
// Compile with: g++ -std=c++17 -o pitfall_livelock pitfall-livelock.cpp

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
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

// --- 1. The livelock pattern -------------------------------------------------
//
// Two workers each own one resource and need the other one as well.  Both are
// polite: when the resource they want is busy they give up what they hold and
// retry.  Because they retry in lockstep they collide again and again, both
// stay busy, and the work is never done.  That is a livelock: no deadlock,
// no progress.

std::mutex res_a;  // only worker A holds it
std::mutex res_b;  // only worker B holds it

constexpr int kMaxRounds = 50;

void polite_worker_a(Barrier& collide) {
    for (int round = 0; round < kMaxRounds; ++round) {
        res_a.lock();                      // take our own resource
        collide.arrive_and_wait();         // (1) both workers hold one resource
        // Worker B still holds res_b at this point, so this can never
        // succeed: the collision is repeated on every single round.
        const bool got_other = res_b.try_lock();
        collide.arrive_and_wait();         // (2) both tried before anyone moves
        if (got_other) {
            res_b.unlock();
            res_a.unlock();
            return;  // unreachable in this schedule
        }
        res_a.unlock();                    // (3) politely step aside
        collide.arrive_and_wait();         // (4) both stepped aside
    }
}

void polite_worker_b(Barrier& collide) {
    for (int round = 0; round < kMaxRounds; ++round) {
        res_b.lock();
        collide.arrive_and_wait();
        const bool got_other = res_a.try_lock();
        collide.arrive_and_wait();
        if (got_other) {
            res_a.unlock();
            res_b.unlock();
            return;  // unreachable in this schedule
        }
        res_b.unlock();
        collide.arrive_and_wait();
    }
}

void livelock_pattern() {
    std::cout << "--- 1. The livelock pattern ---\n";

    Barrier collide(2);
    std::thread a(polite_worker_a, std::ref(collide));
    std::thread b(polite_worker_b, std::ref(collide));
    a.join();
    b.join();

    std::cout << "rounds each worker retried: " << kMaxRounds << "\n";
    std::cout << "work completed: 0 (expected 1)\n";
    std::cout << "both threads were running the whole time - that is the\n"
                 "difference from a deadlock: busy, but no progress\n";
}

// --- 2. Fix 1: randomized backoff -------------------------------------------

std::mutex backoff_a;
std::mutex backoff_b;

constexpr int kMaxBackoffRounds = 400;

void backoff_worker(Barrier& start, bool is_worker_a, unsigned seed) {
    std::mt19937 rng{seed};  // one generator per thread: no shared state
    std::uniform_int_distribution<int> delay_ms(0, 2);
    std::mutex& mine = is_worker_a ? backoff_a : backoff_b;
    std::mutex& other = is_worker_a ? backoff_b : backoff_a;

    start.arrive_and_wait();
    for (int round = 0; round < kMaxBackoffRounds; ++round) {
        mine.lock();
        if (other.try_lock()) {  // the other worker is not holding it right now
            other.unlock();
            mine.unlock();
            return;  // both resources were available: the work is done
        }
        mine.unlock();  // polite: step aside
        // The random delay breaks the symmetry: the two workers no longer
        // wake up at the same moment, so one of them finds the other one idle.
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms(rng)));
    }
}

void randomized_backoff_fix() {
    std::cout << "\n--- 2. Fix 1: randomized backoff ---\n";

    Barrier start(2);
    std::atomic<int> finished{0};

    auto worker = [&](bool is_worker_a, unsigned seed) {
        backoff_worker(start, is_worker_a, seed);
        ++finished;
    };

    std::thread a(worker, true, 12345u);   // fixed seeds: the demo is reproducible
    std::thread b(worker, false, 54321u);
    a.join();
    b.join();

    std::cout << "workers that got both resources: " << finished.load()
              << " (expected 2)\n";
    std::cout << "a random delay breaks the tie instead of synchronizing it\n";
}

// --- 3. Fix 2: one clear owner ----------------------------------------------

std::mutex owner_mutex;
bool job_done = false;
int job_runs = 0;

void single_owner_worker(Barrier& start) {
    start.arrive_and_wait();
    // One mutex covers the whole check-then-act sequence, so the second
    // worker sees the finished job instead of repeating it.
    std::lock_guard<std::mutex> lock(owner_mutex);
    if (!job_done) {
        job_done = true;
        ++job_runs;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));  // the "expensive" part
    }
}

void single_owner_fix() {
    std::cout << "\n--- 3. Fix 2: one clear owner ---\n";

    Barrier start(2);
    std::thread a(single_owner_worker, std::ref(start));
    std::thread b(single_owner_worker, std::ref(start));
    a.join();
    b.join();

    std::cout << "times the job ran: " << job_runs << " (expected 1)\n";
    std::cout << "a mutex or condition variable removes the hand-rolled\n"
                 "yield-and-retry dance entirely\n";
}

// --- 4. How the symptom looks -----------------------------------------------

void symptom() {
    std::cout << "\n--- 4. How a livelock looks ---\n";
    std::cout << "the threads are alive and scheduled, so the CPU is busy,\n"
                 "but no result ever appears: 100% CPU and no output is the\n"
                 "classic livelock smell\n";
}

int main() {
    livelock_pattern();
    randomized_backoff_fix();
    single_owner_fix();
    symptom();

    std::cout << "\n=== All livelock scenarios completed ===\n";
    return 0;
}
