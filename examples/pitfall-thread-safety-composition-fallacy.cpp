// examples/pitfall-thread-safety-composition-fallacy.cpp
// Demonstrates why thread-safe operations do not compose into a thread-safe whole.
// Compile with: g++ -std=c++20 -o pitfall_thread_safety_composition_fallacy pitfall-thread-safety-composition-fallacy.cpp

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

// --- 1. Every operation is safe on its own ---

namespace naive {

std::map<int, std::string> cache;
std::mutex mtx;
std::atomic<int> computations{0};

std::string computeExpensiveValue(int key) {
    computations++;
    return "value:" + std::to_string(key);
}

// Safe by itself: the map is only ever touched under the lock.
std::string getValue(int key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = cache.find(key);
    return it == cache.end() ? std::string() : it->second;
}

// Safe by itself too.
void setValue(int key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    cache[key] = value;
}

}  // namespace naive

// A gate that lets two threads leave at the same time. It turns the
// interleaving from "likely" into "certain", so the example shows the same
// thing every time it runs.
class Gate {
public:
    void arrive() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (++waiting_ == 2) {
            open_ = true;
            condition_.notify_all();
            return;
        }
        condition_.wait(lock, [this] { return open_; });
    }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    int waiting_ = 0;
    bool open_ = false;
};

// --- 2. The composition is not safe ---

std::string getOrComputeRacy(int key, Gate& gate) {
    std::string value = naive::getValue(key);  // the lock is released here
    if (value.empty()) {
        gate.arrive();  // both threads are past the check now
        value = naive::computeExpensiveValue(key);
        naive::setValue(key, value);  // one of the two results wins
    }
    return value;
}

void showTheFallacy() {
    std::cout << "--- 1. Composing two safe operations is not safe ---\n";

    naive::computations = 0;
    Gate gate;
    std::string first;
    std::string second;

    std::thread a([&] { first = getOrComputeRacy(7, gate); });
    std::thread b([&] { second = getOrComputeRacy(7, gate); });
    a.join();
    b.join();

    std::cout << "  both threads read an empty cache and both computed: "
              << naive::computations << " computations for one key\n";
    std::cout << "  the two threads returned \"" << first << "\" and \""
              << second << "\"\n";
    std::cout << "  the expensive work was done twice, and one result was"
                 " thrown away\n";
}

// --- 3. The fix: one lock around the whole compound operation ---

namespace fixed {

std::map<int, std::string> cache;
std::shared_mutex mtx;
std::atomic<int> computations{0};

std::string computeExpensiveValue(int key) {
    computations++;
    return "value:" + std::to_string(key);
}

std::string getOrCompute(int key) {
    // First check: a shared lock, so the readers do not block each other.
    {
        std::shared_lock<std::shared_mutex> readLock(mtx);
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
    }

    // Second check: under the write lock, because somebody else may have
    // filled the entry while this thread was waiting for it.
    std::unique_lock<std::shared_mutex> writeLock(mtx);
    {
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
    }

    std::string value = computeExpensiveValue(key);
    cache[key] = value;
    return value;
}

}  // namespace fixed

void showTheFix() {
    std::cout << "\n--- 2. One lock around the whole compound operation ---\n";

    fixed::computations = 0;

    constexpr int threads = 8;
    std::vector<std::string> results(threads);
    std::vector<std::thread> workers;
    for (int i = 0; i < threads; ++i) {
        workers.emplace_back([&results, i] { results[static_cast<size_t>(i)] = fixed::getOrCompute(7); });
    }
    for (auto& worker : workers) {
        worker.join();
    }

    bool allSame = true;
    for (const auto& result : results) {
        allSame = allSame && result == results.front();
    }

    std::cout << "  " << threads << " threads asked for the same key\n";
    std::cout << "  computations: " << fixed::computations << "\n";
    std::cout << "  every thread got the same value: " << (allSame ? "yes" : "no")
              << "\n";
}

// --- 4. The same thing with std::call_once ---

namespace once {

std::once_flag flag;
std::string value;
std::atomic<int> computations{0};

std::string computeExpensiveValue() {
    computations++;
    return "computed once";
}

std::string getOrCompute() {
    std::call_once(flag, [] { value = computeExpensiveValue(); });
    return value;
}

}  // namespace once

void showCallOnce() {
    std::cout << "\n--- 3. std::call_once for a one time initialisation ---\n";

    once::computations = 0;

    constexpr int threads = 8;
    std::vector<std::thread> workers;
    for (int i = 0; i < threads; ++i) {
        workers.emplace_back([] { once::getOrCompute(); });
    }
    for (auto& worker : workers) {
        worker.join();
    }

    std::cout << "  " << threads << " threads asked for the value\n";
    std::cout << "  computations: " << once::computations << "\n";
    std::cout << "  the value is \"" << once::getOrCompute() << "\"\n";
}

// --- 5. The two shapes the race takes ---

void showTheTrapShapes() {
    std::cout << "\n--- 4. The two shapes the race takes ---\n";

    // Check-then-act: the check and the use are not one operation.
    std::map<int, std::string> values;
    std::mutex valuesMutex;

    // BAD: another thread may erase the key in between the two lines.
    // std::string dangerousGet(int key) {
    //     if (values.contains(key)) {
    //         return values[key];
    //     }
    //     return "";
    // }

    // GOOD: one lock covers the check and the use.
    auto safeGet = [&values, &valuesMutex](int key) {
        std::lock_guard<std::mutex> lock(valuesMutex);
        auto it = values.find(key);
        return it == values.end() ? std::string() : it->second;
    };

    values[1] = "one";
    std::cout << "  check-then-act, fixed: the value is \"" << safeGet(1)
              << "\"\n";

    // Read-modify-write: the load and the store are two operations.
    std::atomic<int> counter{0};

    // BAD: not an atomic operation, another thread may store in between.
    // void dangerousIncrement() {
    //     int value = counter.load();
    //     counter.store(value + 1);
    // }

    // GOOD: fetch_add does the read and the write in one step.
    auto safeIncrement = [&counter] { return counter.fetch_add(1); };

    constexpr int threads = 8;
    constexpr int rounds = 1000;
    std::vector<std::thread> workers;
    for (int i = 0; i < threads; ++i) {
        workers.emplace_back([&safeIncrement] {
            for (int j = 0; j < rounds; ++j) {
                safeIncrement();
            }
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }

    std::cout << "  read-modify-write, fixed: the counter is " << counter.load()
              << " after " << threads * rounds << " increments\n";
}

int main() {
    std::cout << "=== Thread Safety Composition Fallacy ===\n\n";

    showTheFallacy();
    showTheFix();
    showCallOnce();
    showTheTrapShapes();

    std::cout << "\n=== All composition scenarios completed ===\n";
    return 0;
}
