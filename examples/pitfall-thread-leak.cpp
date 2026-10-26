// examples/pitfall-thread-leak.cpp
// Demonstrates thread leaks (a joinable thread destroyed, a detached thread) and the RAII fixes.
// Compile with: g++ -std=c++20 -o pitfall_thread_leak pitfall-thread-leak.cpp

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

// --- 1. Destroying a joinable std::thread ---

void spawnWithoutJoin() {
    std::thread worker([] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });

    // BAD: 'worker' leaves the scope while it is still joinable, so
    // std::thread::~thread calls std::terminate() and the program aborts.
    // worker.join();  <-- the missing call
}

// --- 2. Fix: join explicitly ---

void spawnWithJoin() {
    std::thread worker([] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });
    worker.join();
    std::cout << "  the worker was joined before the thread object died\n";
}

// --- 3. Fix: an RAII wrapper that joins on scope exit ---

class joining_thread {
public:
    explicit joining_thread(std::thread thread) : thread_(std::move(thread)) {}

    joining_thread(const joining_thread&) = delete;
    joining_thread& operator=(const joining_thread&) = delete;

    ~joining_thread() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

private:
    std::thread thread_;
};

void spawnWithJoiningWrapper() {
    joining_thread worker{std::thread([] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); })};
    std::cout << "  the wrapper joins the thread when it leaves the scope\n";
}

// --- 4. Fix: std::jthread joins itself and carries a stop token ---

void spawnWithJthread() {
    std::jthread worker([] { std::this_thread::sleep_for(std::chrono::milliseconds(5)); });
    std::cout << "  the jthread is joined automatically at scope exit\n";
}

void jthreadWithStopToken() {
    std::jthread worker([](std::stop_token token) {
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        std::cout << "  the worker saw the stop request and exited cleanly\n";
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    worker.request_stop();
    // The destructor of the jthread requests the stop and joins the thread.
}

// --- 5. Detaching and forgetting ---

void spawnDetached() {
    int local = 42;
    std::thread worker([&local] { std::cout << local << "\n"; });

    // BAD: 'local' dies at the end of this function while the detached thread
    // may still be running, so reading it is a use after free.
    worker.detach();
}

void spawnDetachedSafely() {
    std::promise<void> done;
    std::future<void> finished = done.get_future();

    std::thread worker([&done] {
        std::cout << "  the detached worker owns everything it captures\n";
        done.set_value();
    });
    worker.detach();

    finished.wait();  // the caller keeps a way to observe the completion
    std::cout << "  the detached worker finished\n";
}

// --- 6. Fix: a thread registry with an explicit shutdown path ---

class WorkerPool {
public:
    void spawn(std::function<void()> job) {
        workers_.emplace_back([job = std::move(job), this] {
            while (!stop_requested_.load(std::memory_order_relaxed)) {
                job();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    ~WorkerPool() {
        stop_requested_.store(true, std::memory_order_relaxed);
        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

private:
    std::vector<std::thread> workers_;
    std::atomic<bool> stop_requested_{false};
};

void threadRegistry() {
    std::atomic<int> completed{0};

    {
        WorkerPool pool;
        for (int i = 0; i < 3; ++i) {
            pool.spawn([&completed] { completed.fetch_add(1, std::memory_order_relaxed); });
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }  // the pool requests the stop and joins every worker here

    std::cout << "  the registry joined its workers, " << completed.load()
              << " jobs were completed\n";
}

int main() {
    std::cout << "--- Destroying a joinable thread ---\n";
    // BAD: this would call std::terminate() and abort the program.
    // spawnWithoutJoin();
    std::cout << "  the aborting call is commented out, see the comment above\n";

    std::cout << "\n--- Fix: join explicitly ---\n";
    spawnWithJoin();

    std::cout << "\n--- Fix: joining RAII wrapper ---\n";
    spawnWithJoiningWrapper();

    std::cout << "\n--- Fix: std::jthread ---\n";
    spawnWithJthread();

    std::cout << "\n--- Fix: std::jthread with a stop token ---\n";
    jthreadWithStopToken();

    std::cout << "\n--- Detaching and forgetting ---\n";
    // BAD: the detached thread would read a local that is already gone.
    // spawnDetached();
    std::cout << "  the use after free is commented out, see the comment above\n";
    spawnDetachedSafely();

    std::cout << "\n--- Fix: thread registry with a shutdown path ---\n";
    threadRegistry();

    std::cout << "\n=== All thread leak scenarios completed ===\n";
    return 0;
}
