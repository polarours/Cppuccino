#include "actor_mailbox.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

using namespace std::chrono_literals;

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_single_threaded_order() {
    actor::Mailbox<int> mb;
    std::vector<int> seen;
    mb.start([&](int& v) { seen.push_back(v); });
    for (int i = 1; i <= 5; ++i) mb.send(i);
    // wait for drain
    for (int i = 0; i < 200 && mb.pending() > 0; ++i) std::this_thread::sleep_for(2ms);
    mb.stop();
    expect((seen == std::vector<int>{1, 2, 3, 4, 5}), "FIFO order preserved");
    expect(mb.handled() == 5, "5 handled");
}

void test_bounded_backpressure() {
    actor::Mailbox<int> mb(4);
    expect(mb.send(1), "1");
    expect(mb.send(2), "2");
    expect(mb.send(3), "3");
    expect(mb.send(4), "4");
    expect(!mb.send(5), "5th rejected when full (no handler yet)");
    expect(mb.pending() == 4, "stays at bound");
}

void test_handler_sees_all_before_stop() {
    actor::Mailbox<int> mb;
    std::atomic<int> count{0};
    mb.start([&](int&) { count++; });
    for (int i = 0; i < 100; ++i) mb.send(i);
    // stop() drains? No - stop discards; wait first.
    for (int i = 0; i < 500 && mb.pending() > 0; ++i) std::this_thread::sleep_for(2ms);
    mb.stop();
    expect(count.load() == 100, "all 100 handled before stop");
}

void test_handler_runs_without_lock() {
    // Reentrancy: handler sending to ANOTHER mailbox must not deadlock
    actor::Mailbox<int> a, b;
    std::atomic<int> bCount{0};
    b.start([&](int&) { bCount++; });
    a.start([&](int& v) { b.send(v + 1000); });  // send while handling
    a.send(1);
    for (int i = 0; i < 200 && b.pending() > 0; ++i) std::this_thread::sleep_for(2ms);
    for (int i = 0; i < 200 && bCount.load() == 0; ++i) std::this_thread::sleep_for(2ms);
    a.stop();
    b.stop();
    expect(bCount.load() == 1, "reentrant send handled");
}

void test_mpmc_no_loss_no_dup() {
    actor::Mailbox<long> mb(256);
    std::atomic<long> sum{0};
    mb.start([&](long& v) { sum += v; });

    constexpr int kProducers = 4, kPer = 250;
    std::vector<std::thread> producers;
    for (int p = 0; p < kProducers; ++p) {
        producers.emplace_back([&mb, p] {
            for (int i = 0; i < kPer; ++i) {
                while (!mb.send(static_cast<long>(p * kPer + i))) {
                    std::this_thread::sleep_for(100us);  // backpressure
                }
            }
        });
    }
    for (auto& t : producers) t.join();
    for (int i = 0; i < 1000 && mb.pending() > 0; ++i) std::this_thread::sleep_for(2ms);
    mb.stop();

    long n = kProducers * kPer;
    long expected = n * (n - 1) / 2;  // 0..n-1 sum
    expect(mb.handled() == n, "exactly n handled (no loss/dup)");
    expect(sum.load() == expected, "sum intact");
}

void test_stop_idempotent_and_send_after_stop() {
    actor::Mailbox<int> mb;
    mb.start([](int&) {});
    mb.stop();
    mb.stop();  // second stop no-op
    expect(!mb.send(1), "send after stop rejected");
    expect(!mb.running(), "not running");
}

}  // namespace

int main() {
    try {
        test_single_threaded_order();
        test_bounded_backpressure();
        test_handler_sees_all_before_stop();
        test_handler_runs_without_lock();
        test_mpmc_no_loss_no_dup();
        test_stop_idempotent_and_send_after_stop();
        std::cout << "All actor_mailbox tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
