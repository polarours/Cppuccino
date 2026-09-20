// examples/web-server-benchmark.cpp
// Real HTTP benchmark for the web_server project.
//
// Compile + run:
//   g++ -std=c++17 -pthread -Wall -Wextra -Icpp-mastery-roadmap/04-stage4/web-server/include
//       -o /tmp/web-server-benchmark examples/web-server-benchmark.cpp
//       cpp-mastery-roadmap/04-stage4/web-server/src/web_server.cpp
//   /tmp/web-server-benchmark
//
// Drives REAL TCP HTTP requests against a running WebServer, verifies the
// response body, and reports throughput + p50/p99 latency per concurrency.

#include "web_server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace {

struct HttpResult {
    int statusCode;
    std::string body;
    double latencyMs;
};

// Minimal HTTP/1.1 client: connect, send one GET, read until close.
HttpResult httpGet(int port, const std::string& path) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return {599, "socket failed", 0};
    }

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));
    ::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    auto start = std::chrono::steady_clock::now();
    if (::connect(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) != 0) {
        ::close(fd);
        return {599, "connect failed", 0};
    }

    std::string req = "GET " + path + " HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    if (::send(fd, req.data(), req.size(), 0) != static_cast<ssize_t>(req.size())) {
        ::close(fd);
        return {599, "send failed", 0};
    }

    std::string data;
    char buf[4096];
    while (true) {
        ssize_t n = ::recv(fd, buf, sizeof(buf), 0);
        if (n <= 0) break;
        data.append(buf, static_cast<size_t>(n));
    }
    ::close(fd);
    auto end = std::chrono::steady_clock::now();

    int status = 599;
    auto statusEnd = data.find("\r\n");
    if (statusEnd != std::string::npos && data.rfind("HTTP/1.1 ", 0, 9) == 0) {
        status = std::atoi(data.c_str() + 8);
    }
    auto bodyStart = data.find("\r\n\r\n");
    std::string body = (bodyStart != std::string::npos) ? data.substr(bodyStart + 4) : "";

    return {status, body,
            std::chrono::duration<double, std::milli>(end - start).count()};
}

struct BenchmarkReport {
    int concurrency;
    int total;
    int errors;
    double reqPerSec;
    double p50Us;
    double p99Us;
};

BenchmarkReport runRound(int port, const std::string& path,
                         int concurrency, int totalRequests) {
    auto results = std::make_shared<std::vector<double>>(totalRequests);
    auto statuses = std::make_shared<std::vector<int>>(totalRequests);
    auto start = std::chrono::steady_clock::now();

    std::vector<std::thread> workers;
    for (int c = 0; c < concurrency; ++c) {
        workers.emplace_back([&, c]() {
            for (int i = c; i < totalRequests; i += concurrency) {
                auto r = httpGet(port, path);
                (*results)[i] = r.latencyMs * 1000.0; // us
                (*statuses)[i] = r.statusCode;
                if (r.body != "pong-ok") (*statuses)[i] = 598;
            }
        });
    }
    for (auto& w : workers) w.join();
    auto end = std::chrono::steady_clock::now();

    int errors = 0;
    for (int s : *statuses) {
        if (s != 200) ++errors;
    }

    auto sorted = *results;
    std::sort(sorted.begin(), sorted.end());
    double p50 = sorted[sorted.size() / 2];
    double p99 = sorted[std::min<size_t>(sorted.size() - 1,
                                          static_cast<size_t>(totalRequests * 0.99))];

    double seconds = std::chrono::duration<double>(end - start).count();
    return {concurrency, totalRequests, errors,
            seconds > 0 ? totalRequests / seconds : 0, p50, p99};
}

} // namespace

int main() {
    constexpr int kPort = 8790;
    const std::string token = "pong-ok";

    web_server::WebServer server(kPort);
    server.get("/ping", [&token](const web_server::Request&) {
        return web_server::Response::ok(token);
    });

    // start() blocks on accept(); stop() closes the listening socket so the
    // accept loop exits. Run the server on its own thread.
    std::thread serverThread([&server]() { server.start(); });

    // Wait until the server answers real requests.
    bool ready = false;
    for (int i = 0; i < 50; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto probe = httpGet(kPort, "/ping");
        if (probe.statusCode == 200 && probe.body == token) {
            ready = true;
            break;
        }
    }
    if (!ready) {
        std::cerr << "server never became ready on port " << kPort << "\n";
        server.stop();
        serverThread.detach();
        return 1;
    }

    std::cout << "=== Web Server Benchmark (real HTTP over TCP) ===\n";
    std::cout << "port=" << kPort << " path=/ping\n\n";

    const int kTotal = 2000;
    int round = 0;
    int failed = 0;
    for (int concurrency : {1, 4, 8, 16}) {
        auto r = runRound(kPort, "/ping", concurrency, kTotal);
        ++round;
        std::cout << "[" << round << "] concurrency=" << r.concurrency
                  << " requests=" << r.total
                  << " errors=" << r.errors
                  << " throughput=" << static_cast<int>(std::round(r.reqPerSec))
                  << " req/s p50=" << static_cast<int>(std::round(r.p50Us))
                  << "us p99=" << static_cast<int>(std::round(r.p99Us))
                  << "us" << std::endl;
        if (r.errors != 0) failed = 1;
    }

    // stop() closes the listening socket, which on Linux does NOT wake a
    // thread blocked in accept(); the server thread would hang its join
    // forever. Detach it — the process exits immediately, which kills it.
    server.stop();
    serverThread.detach();

    std::cout << "\n=== Benchmark Complete ===\n";
    return failed;
}
