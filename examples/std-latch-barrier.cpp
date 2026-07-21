#include <latch>
#include <barrier>
#include <thread>
#include <iostream>
#include <vector>
#include <string>

// std::latch - one-shot countdown synchronization
// std::barrier - reusable phase synchronization
// C++20: <latch>, <barrier>

int main() {
    std::cout << "=== std::latch & std::barrier (C++20) ===\n\n";

    // std::latch: one-shot countdown
    std::cout << "Latch example:\n";
    {
        constexpr int threadCount = 4;
        std::latch latch(threadCount);
        std::vector<int> results(threadCount);

        std::vector<std::jthread> threads;
        for (int i = 0; i < threadCount; ++i) {
            threads.emplace_back([&latch, &results, i]() {
                results[i] = i * i;
                latch.count_down();  // decrement
            });
        }

        latch.wait();  // block until count reaches 0
        std::cout << "  Results: ";
        for (int r : results) std::cout << r << " ";
        std::cout << "\n";
    }

    // std::barrier: reusable phase sync
    std::cout << "\nBarrier example:\n";
    {
        constexpr int threadCount = 3;
        int phase = 0;

        std::barrier barrier(threadCount, [&phase]() {
            phase++;
            std::cout << "  Phase " << phase << " complete\n";
        });

        std::vector<std::jthread> threads;
        for (int i = 0; i < threadCount; ++i) {
            threads.emplace_back([&barrier, i]() {
                for (int p = 0; p < 2; ++p) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    std::cout << "  Thread " << i << " phase " << (p+1) << " done\n";
                    barrier.arrive_and_wait();  // wait for all threads
                }
            });
        }
    }

    std::cout << "\nAll sync complete\n";
    return 0;
}
