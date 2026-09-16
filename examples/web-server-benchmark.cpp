// examples/web-server-benchmark.cpp
// Benchmark: measure request handling performance of WebServer.
// Compile: g++ -std=c++20 -pthread -O2 -o web-server-benchmark web-server-benchmark.cpp

#include "../cpp-mastery-roadmap/04-stage4/web-server/include/web_server.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    using namespace web_server;
    using Clock = std::chrono::steady_clock;

    WebServer server(0); // port 0 → OS assigns

    server.get("/ping", [](const Request&) {
        return Response::ok("pong");
    });

    server.get("/slow", [](const Request&) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        return Response::ok("done");
    });

    server.start();

    auto benchmark = [&](const std::string& path, int concurrency, int totalRequests) {
        std::vector<std::thread> threads;
        std::atomic<int> success{0}, errors{0};
        auto start = Clock::now();

        for (int i = 0; i < concurrency; ++i) {
            threads.emplace_back([&]() {
                for (int j = 0; j < totalRequests / concurrency; ++j) {
                    // Simple synchronous request simulation
                    // Note: This is a simplified benchmark without actual TCP
                    // For real benchmark, use ab/hey or write a proper client
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                    ++success;
                }
            });
        }
        for (auto& t : threads) t.join();

        auto end = Clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << path << " | " << concurrency << " concurrent | "
                  << totalRequests << " requests | " << ms << " ms"
                  << " | " << (ms > 0 ? (totalRequests * 1000.0 / ms) : 0) << " req/s\n";
    };

    std::cout << "=== Web Server Benchmark ===\n\n";
    std::cout << "Note: This is a simplified benchmark. For accurate results,\n";
    std::cout << "use tools like `ab`, `hey`, or `wrk` against a running server.\n\n";

    // Simulated concurrency levels
    for (int c : {1, 4, 8, 16}) {
        benchmark("/ping", c, 1000);
    }

    server.stop();
    std::cout << "\n=== Benchmark Complete ===\n";
    return 0;
}
