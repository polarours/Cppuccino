// examples/pitfall-the-myth-of-thread-safety.cpp
// Demonstrates why const, atomics and per-object locks are not enough on their own.
// Compile with: g++ -std=c++17 -o pitfall_the_myth_of_thread_safety pitfall-the-myth-of-thread-safety.cpp

#include <atomic>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// --- 1. const does not make an object thread-safe ---

class Config {
public:
    explicit Config(std::string name) : name_(std::move(name)) {}
    const std::string& name() const { return name_; }   // a const accessor ...
    void rename(std::string name) { name_ = std::move(name); }  // ... and a mutator

private:
    std::string name_;
};

class SafeConfig {
public:
    explicit SafeConfig(std::string name) : name_(std::move(name)) {}

    std::string name() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return name_;
    }

    void rename(std::string name) {
        std::lock_guard<std::mutex> lock(mtx_);
        name_ = std::move(name);
    }

private:
    mutable std::mutex mtx_;
    std::string name_;
};

void constIsNotThreadSafety() {
    std::cout << "--- 1. const does not mean thread-safe ---\n";

    Config config("server");
    const Config& view = config;  // a const *view* of a mutable object
    std::cout << "  view.name() = " << view.name() << "\n";

    // BAD: if another thread calls rename() while this thread reads name(),
    // the read and the write race -- and the const accessor does nothing to
    // prevent that. Two threads touching a std::string concurrently is
    // undefined behaviour, so it is not run here.

    // GOOD: guard the state with a mutex.
    SafeConfig safe("server");
    safe.rename("server-2");
    std::cout << "  guarded name()  = " << safe.name() << "\n";
}

// --- 2. A compound read-modify-write loses updates ---

// A deterministic simulation of two threads interleaving read-modify-write.
// No threads are needed: the interleaving is written out by hand.
int simulateLostUpdate() {
    int value = 0;
    const int t1 = value;  // thread 1 reads 0
    const int t2 = value;  // thread 2 reads 0
    value = t1 + 1;        // thread 1 writes 1
    value = t2 + 1;        // thread 2 writes 1
    return value;          // 1 -- one of the two increments was lost
}

class AtomicCounter {
public:
    void increment() { value_.fetch_add(1); }  // one atomic read-modify-write
    int value() const { return value_.load(); }

private:
    std::atomic<int> value_{0};
};

void compoundOperations() {
    std::cout << "\n--- 2. Compound read-modify-write ---\n";

    std::cout << "  simulated interleaving loses an update: final value = "
              << simulateLostUpdate() << " (two increments were made)\n";

    AtomicCounter counter;
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&counter]() {
            for (int n = 0; n < 10000; ++n) {
                counter.increment();
            }
        });
    }
    for (std::thread& thread : threads) {
        thread.join();
    }
    std::cout << "  atomic counter after 4 x 10000 increments: " << counter.value() << "\n";
}

// --- 3. One lock per object is not enough ---

struct Account {
    std::mutex mtx;
    int balance = 0;
};

// BAD: taking one lock and then the other deadlocks when two threads
// transfer money in opposite directions.
// void transferOneLockAtATime(Account& from, Account& to, int amount) {
//     std::lock_guard<std::mutex> first(from.mtx);
//     std::lock_guard<std::mutex> second(to.mtx);
//     from.balance -= amount;
//     to.balance += amount;
// }

// GOOD: std::scoped_lock takes both locks at once, in a deadlock-free order.
void transfer(Account& from, Account& to, int amount) {
    std::scoped_lock lock(from.mtx, to.mtx);
    from.balance -= amount;
    to.balance += amount;
}

void lockOrdering() {
    std::cout << "\n--- 3. One lock per object is not enough ---\n";

    Account left;
    Account right;
    left.balance = 100;

    std::thread t1([&left, &right]() { transfer(left, right, 10); });
    std::thread t2([&left, &right]() { transfer(right, left, 10); });
    t1.join();
    t2.join();

    std::cout << "  left.balance = " << left.balance << ", right.balance = " << right.balance
              << "\n";
}

// --- 4. Memory ordering ---

void memoryOrdering() {
    std::cout << "\n--- 4. Memory ordering ---\n";

    std::atomic<bool> ready{false};
    std::atomic<int> data{0};

    std::thread producer([&ready, &data]() {
        data.store(42, std::memory_order_release);
        ready.store(true, std::memory_order_release);
    });
    std::thread consumer([&ready, &data]() {
        while (!ready.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        std::cout << "  consumer saw data = " << data.load(std::memory_order_acquire) << "\n";
    });
    producer.join();
    consumer.join();

    // BAD: a plain `int data` written by one thread and read by another is a
    // data race -- the atomic plus the acquire/release pairing is what makes
    // the handshake safe.
}

// --- 5. Check-then-act on an atomic is still a race ---

bool tryAcquire(std::atomic<int>& slots) {
    // BAD: the load and the store are two separate steps; another thread can
    // slip in between them and take the same slot.
    const int current = slots.load();
    if (current > 0) {
        slots.store(current - 1);
        return true;
    }
    return false;
}

bool tryAcquireSafe(std::atomic<int>& slots) {
    // GOOD: one atomic compare-and-exchange retries until it succeeds.
    int current = slots.load();
    while (current > 0) {
        if (slots.compare_exchange_weak(current, current - 1)) {
            return true;
        }
    }
    return false;
}

void checkThenAct() {
    std::cout << "\n--- 5. Check-then-act on an atomic is still a race ---\n";

    std::atomic<int> slots{1};
    std::cout << "  tryAcquireSafe took the last slot: " << (tryAcquireSafe(slots) ? "yes" : "no")
              << "\n";
    std::cout << "  tryAcquireSafe on an empty pool   : " << (tryAcquireSafe(slots) ? "yes" : "no")
              << "\n";
}

// --- 6. The best synchronization is no sharing ---

void threadLocalState() {
    std::cout << "\n--- 6. Prefer not to share at all ---\n";

    thread_local int perThread = 0;
    auto worker = [](int id) {
        perThread += id;  // every thread has its own copy
        std::cout << "  thread " << id << " sees perThread = " << perThread << "\n";
    };

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    t1.join();
    t2.join();
}

int main() {
    constIsNotThreadSafety();
    compoundOperations();
    lockOrdering();
    memoryOrdering();
    checkThenAct();
    threadLocalState();

    std::cout << "\n=== All thread safety scenarios completed ===\n";
    return 0;
}
