// examples/pitfall-memory-order-pitfalls.cpp
// Demonstrates why a relaxed atomic handshake is not a synchronisation point and how acquire/release fixes it.
// Compile with: g++ -std=c++17 -o pitfall_memory_order_pitfalls pitfall-memory-order-pitfalls.cpp

#include <atomic>
#include <iostream>
#include <thread>

constexpr int kHandshakeRounds = 1000;

// --- 1. The broken pattern: a relaxed handshake ---

// The standard orders neither the two stores nor the two loads against each
// other, so observing flag == 1 does not guarantee that 'payload' is visible.
// On a weakly ordered CPU the reader can still read the old payload.
void relaxedHandshake(int& observed) {
    std::atomic<int> payload{0};
    std::atomic<int> flag{0};

    std::thread writer([&] {
        payload.store(42, std::memory_order_relaxed);
        flag.store(1, std::memory_order_relaxed);
    });

    std::thread reader([&] {
        while (flag.load(std::memory_order_relaxed) == 0) {
            std::this_thread::yield();
        }
        observed = payload.load(std::memory_order_relaxed);
    });

    writer.join();
    reader.join();
}

// --- 2. The fix: release publishes, acquire observes ---

void acquireReleaseHandshake(int& observed) {
    std::atomic<int> payload{0};
    std::atomic<int> flag{0};

    std::thread writer([&] {
        payload.store(42, std::memory_order_relaxed);  // the plain data write
        flag.store(1, std::memory_order_release);      // publishes it
    });

    std::thread reader([&] {
        // The acquire load that observes the release store synchronizes with
        // it, so everything the writer did before the release is visible.
        while (flag.load(std::memory_order_acquire) == 0) {
            std::this_thread::yield();
        }
        observed = payload.load(std::memory_order_relaxed);
    });

    writer.join();
    reader.join();
}

// --- 3. The fix with explicit fences ---

void fenceHandshake(int& observed) {
    std::atomic<int> payload{0};
    std::atomic<int> flag{0};

    std::thread writer([&] {
        payload.store(42, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_release);
        flag.store(1, std::memory_order_relaxed);
    });

    std::thread reader([&] {
        while (flag.load(std::memory_order_relaxed) == 0) {
            std::this_thread::yield();
        }
        std::atomic_thread_fence(std::memory_order_acquire);
        observed = payload.load(std::memory_order_relaxed);
    });

    writer.join();
    reader.join();
}

// --- 4. The default: sequential consistency ---

void seqCstHandshake(int& observed) {
    std::atomic<int> payload{0};
    std::atomic<int> flag{0};

    std::thread writer([&] {
        payload.store(42, std::memory_order_relaxed);
        flag.store(1, std::memory_order_seq_cst);
    });

    std::thread reader([&] {
        while (flag.load(std::memory_order_seq_cst) == 0) {
            std::this_thread::yield();
        }
        observed = payload.load(std::memory_order_seq_cst);
    });

    writer.join();
    reader.join();
}

// --- 5. Where relaxed is the right choice ---

void relaxedCounter() {
    std::atomic<int> counter{0};

    std::thread first([&] {
        for (int i = 0; i < 10000; ++i) {
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    });
    std::thread second([&] {
        for (int i = 0; i < 10000; ++i) {
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    });

    first.join();
    second.join();

    std::cout << "  relaxed counter total: " << counter.load(std::memory_order_relaxed)
              << " (exact, because fetch_add is atomic and no data is published)\n";
}

int main() {
    int stale_observations = 0;
    int observed = 0;

    std::cout << "--- Relaxed handshake (not a synchronisation point) ---\n";
    for (int round = 0; round < kHandshakeRounds; ++round) {
        relaxedHandshake(observed);
        if (observed != 42) {
            ++stale_observations;
        }
    }
    std::cout << "  rounds: " << kHandshakeRounds
              << ", rounds where the reader saw a stale payload: " << stale_observations
              << "\n";
    if (stale_observations == 0) {
        std::cout << "  this run happened to work: the platform stores are ordered,\n"
                     "  but the C++ standard does not guarantee it. This is exactly the\n"
                     "  'works in tests' trap the article warns about.\n";
    }

    std::cout << "\n--- Release / acquire handshake (guaranteed) ---\n";
    acquireReleaseHandshake(observed);
    std::cout << "  payload observed after the handshake: " << observed << "\n";

    std::cout << "\n--- Fence based handshake (guaranteed) ---\n";
    fenceHandshake(observed);
    std::cout << "  payload observed after the handshake: " << observed << "\n";

    std::cout << "\n--- Sequentially consistent handshake (safe default) ---\n";
    seqCstHandshake(observed);
    std::cout << "  payload observed after the handshake: " << observed << "\n";

    std::cout << "\n--- Relaxed is fine for a counter ---\n";
    relaxedCounter();

    std::cout << "\n=== All memory order scenarios completed ===\n";
    return 0;
}
