#include "blocking_queue.hpp"

#include <iostream>
#include <thread>
#include <vector>

int main() {
    blocking_queue::BlockingQueue<int> q(4);  // small to force blocking

    std::thread producer([&] {
        for (int i = 0; i < 8; ++i) {
            q.push(i);
            std::cout << "push " << i << " (size=" << q.size() << ")\n";
        }
        q.close();
    });

    long total = 0;
    std::thread consumer([&] {
        while (auto v = q.pop(std::chrono::milliseconds(500))) {
            total += *v;
        }
        std::cout << "consumer done, sum=" << total << "\n";
    });

    producer.join();
    consumer.join();
    std::cout << "expected sum 0..7 = 28 -> " << (total == 28 ? "OK" : "WRONG") << "\n";
    return 0;
}
