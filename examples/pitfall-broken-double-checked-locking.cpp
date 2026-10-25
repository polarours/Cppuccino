// examples/pitfall-broken-double-checked-locking.cpp
// Demonstrates broken double-checked locking and the safe replacements.
// Compile with: g++ -std=c++17 -o pitfall_broken_double_checked_locking pitfall-broken-double-checked-locking.cpp

#include <atomic>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// The object that gets published. The constructor writes three members, so a
// reader that observes the pointer before the constructor finished may see an
// inconsistent object.
struct Config {
    int width;
    int height;
    int area;

    Config() : width(640), height(480), area(width * height) {}
};

bool published_consistently(const Config* c) {
    return c != nullptr && c->area == c->width * c->height;
}

// --- 1. The classic broken form -------------------------------------------

Config* g_broken_ptr = nullptr;
std::mutex g_broken_mutex;

Config* instance_broken() {
    if (g_broken_ptr == nullptr) {        // BAD: unsynchronized read
        std::lock_guard<std::mutex> lock(g_broken_mutex);
        if (g_broken_ptr == nullptr) {
            g_broken_ptr = new Config();  // BAD: published without an ordering
        }
    }
    return g_broken_ptr;
}

// --- 2. The same shape with relaxed atomics: still wrong ------------------

std::atomic<Config*> g_relaxed_ptr{nullptr};

Config* instance_relaxed_broken() {
    Config* p = g_relaxed_ptr.load(std::memory_order_relaxed);
    if (p == nullptr) {
        std::lock_guard<std::mutex> lock(g_broken_mutex);
        p = g_relaxed_ptr.load(std::memory_order_relaxed);
        if (p == nullptr) {
            p = new Config();
            g_relaxed_ptr.store(p, std::memory_order_relaxed);
        }
    }
    return p;
}

// --- 3. The correct form: acquire load, release store ---------------------

std::atomic<Config*> g_atomic_ptr{nullptr};
std::unique_ptr<Config> g_atomic_owner;
std::mutex g_atomic_mutex;
std::atomic<int> g_atomic_constructions{0};

Config* instance_correct_dclp() {
    // acquire: pairs with the release store, so the object is fully published
    Config* p = g_atomic_ptr.load(std::memory_order_acquire);
    if (p == nullptr) {
        std::lock_guard<std::mutex> lock(g_atomic_mutex);
        p = g_atomic_ptr.load(std::memory_order_relaxed);
        if (p == nullptr) {
            g_atomic_owner = std::make_unique<Config>();
            p = g_atomic_owner.get();
            g_atomic_constructions.fetch_add(1, std::memory_order_relaxed);
            // release: everything the constructor wrote happens-before a
            // reader that acquires this pointer
            g_atomic_ptr.store(p, std::memory_order_release);
        }
    }
    return p;
}

// --- 4. Replacement: function-local static --------------------------------

std::atomic<int> g_static_constructions{0};

struct StaticConfig {
    int width;
    int height;
    int area;

    StaticConfig() : width(640), height(480), area(width * height) {
        g_static_constructions.fetch_add(1, std::memory_order_relaxed);
    }
};

StaticConfig& instance_local_static() {
    // C++11 guarantees that this initialization happens exactly once, even
    // when several threads call it at the same time
    static StaticConfig obj;
    return obj;
}

// --- 5. Replacement: std::call_once ---------------------------------------

std::once_flag g_once;
std::unique_ptr<Config> g_once_owner;
std::atomic<int> g_once_constructions{0};

Config& instance_call_once() {
    std::call_once(g_once, [] {
        g_once_owner = std::make_unique<Config>();
        g_once_constructions.fetch_add(1, std::memory_order_relaxed);
    });
    return *g_once_owner;
}

// --- 6. The broken forms, described but not run ---------------------------

void broken_double_checked_locking() {
    std::cout << "--- Broken DCLP (not run: undefined behaviour) ---\n";

    // Calling instance_broken() from two threads at once is a data race: the
    // unsynchronized read of g_broken_ptr races with the write inside the
    // lock, and no happens-before edge exists between them.  A reader can
    // also see the pointer before the constructor's writes are visible, so
    // it can observe a half-built object.
    //
    // std::thread t([] { Config* c = instance_broken(); (void)c; });

    // Using relaxed atomics removes the race on the pointer itself, but it
    // still creates no happens-before edge, so the constructor's writes may
    // remain invisible to the reader.
    //
    // std::thread t([] { Config* c = instance_relaxed_broken(); (void)c; });

    std::cout << "The two broken variants are left unexecuted: both are\n"
                 "undefined behaviour, so running them proves nothing.\n";
}

// --- 7. The correct forms, exercised under contention --------------------

constexpr int kThreads = 8;

// Runs body on kThreads threads at once and joins them all.
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

// True when every thread observed the same pointer.
template <typename T>
bool all_same(const std::vector<T*>& seen) {
    for (const T* p : seen) {
        if (p != seen.front()) {
            return false;
        }
    }
    return true;
}

void correct_double_checked_locking() {
    std::cout << "\n--- Correct DCLP: acquire load, release store ---\n";

    std::vector<const Config*> seen(kThreads, nullptr);
    std::atomic<bool> consistent{true};

    run_under_contention([&seen, &consistent](int index) {
        for (int step = 0; step < 1000; ++step) {
            const Config* c = instance_correct_dclp();
            if (!published_consistently(c)) {
                consistent.store(false, std::memory_order_relaxed);
            }
            seen[static_cast<std::size_t>(index)] = c;
        }
    });

    std::cout << "constructions: " << g_atomic_constructions.load() << " (expected 1)\n";
    std::cout << "all threads saw the same object: " << (all_same(seen) ? "yes" : "no") << "\n";
    std::cout << "every observation was consistent: "
              << (consistent.load() ? "yes" : "no") << "\n";
}

void local_static_initialization() {
    std::cout << "\n--- Replacement: function-local static ---\n";

    std::vector<const StaticConfig*> seen(kThreads, nullptr);

    run_under_contention([&seen](int index) {
        seen[static_cast<std::size_t>(index)] = &instance_local_static();
    });

    std::cout << "constructions: " << g_static_constructions.load() << " (expected 1)\n";
    std::cout << "all threads saw the same object: " << (all_same(seen) ? "yes" : "no") << "\n";
}

void call_once_initialization() {
    std::cout << "\n--- Replacement: std::call_once ---\n";

    std::vector<const Config*> seen(kThreads, nullptr);

    run_under_contention([&seen](int index) {
        seen[static_cast<std::size_t>(index)] = &instance_call_once();
    });

    std::cout << "constructions: " << g_once_constructions.load() << " (expected 1)\n";
    std::cout << "all threads saw the same object: " << (all_same(seen) ? "yes" : "no") << "\n";
}

// --- 8. The smell test a reviewer should apply ----------------------------

void review_smell_test() {
    std::cout << "\n--- Review checklist ---\n";
    std::cout << "1. is any shared pointer read outside a lock?   -> broken\n";
    std::cout << "2. is publication proven by acquire/release?    -> required\n";
    std::cout << "3. is duplicate construction impossible?         -> yes\n";
    std::cout << "4. is the destruction policy explicit?           -> yes\n";
    std::cout << "5. could a standard mechanism replace it?       -> yes\n";
}

int main() {
    broken_double_checked_locking();
    correct_double_checked_locking();
    local_static_initialization();
    call_once_initialization();
    review_smell_test();

    std::cout << "\n=== All double-checked locking scenarios completed ===\n";
    return 0;
}
