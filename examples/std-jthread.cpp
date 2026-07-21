#include <thread>
#include <iostream>
#include <string>
#include <chrono>
#include <atomic>

// std::jthread - cooperative cancellation thread
// std::stop_token - stop request for cooperative cancellation
// C++20: <thread>

void longRunningTask(std::stop_token stopToken, int id) {
    int iterations = 0;
    while (!stopToken.stop_requested()) {
        iterations++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (iterations % 5 == 0) {
            std::cout << "  Task " << id << " iteration " << iterations << "\n";
        }
    }
    std::cout << "  Task " << id << " stopped after " << iterations << " iterations\n";
}

void producer(std::stop_token stopToken, std::vector<int>& buffer) {
    for (int i = 0; i < 100; ++i) {
        if (stopToken.stop_requested()) break;
        buffer.push_back(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int main() {
    std::cout << "=== std::jthread (C++20) ===\n\n";

    // jthread auto-joins on destruction
    {
        std::jthread t1(longRunningTask, 1);
        std::jthread t2(longRunningTask, 2);

        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        std::cout << "Requesting stop...\n";
        // t1 and t2 auto-join when going out of scope
    }
    std::cout << "All threads stopped (auto-joined)\n\n";

    // Cooperative cancellation example
    std::cout << "Cooperative cancellation:\n";
    {
        std::vector<int> buffer;
        std::jthread producerThread(producer, std::ref(buffer));

        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        std::cout << "  Collected " << buffer.size() << " items before stop\n";
        // Thread auto-joins on scope exit
    }

    // stop_callback example
    std::cout << "\nStop callback:\n";
    {
        std::jthread t([](std::stop_token st) {
            while (!st.stop_requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            std::cout << "  Cleanup done\n";
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "Thread cleaned up\n";

    return 0;
}
