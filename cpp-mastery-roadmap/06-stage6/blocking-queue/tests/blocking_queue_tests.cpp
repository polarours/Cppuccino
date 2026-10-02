#include "blocking_queue.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

using namespace std::chrono;

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_fifo_order() {
    blocking_queue::BlockingQueue<int> q(4);
    q.push(1); q.push(2); q.push(3);
    expect(q.pop().value_or(-1) == 1, "FIFO 1");
    expect(q.pop().value_or(-1) == 2, "FIFO 2");
    expect(q.pop().value_or(-1) == 3, "FIFO 3");
    expect(q.size() == 0, "drained");
}

void test_bounded_push_timeout() {
    blocking_queue::BlockingQueue<int> q(1);
    expect(q.push(1), "first push ok");
    auto t0 = steady_clock::now();
    bool ok = q.push(2, milliseconds(80));
    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - t0).count();
    expect(!ok, "push on full queue times out");
    expect(elapsed >= 60, "waited ~80ms (got " + std::to_string(elapsed) + ")");
}

void test_pop_timeout_when_empty() {
    blocking_queue::BlockingQueue<int> q(2);
    auto t0 = steady_clock::now();
    auto v = q.pop(milliseconds(60));
    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - t0).count();
    expect(!v.has_value(), "empty pop times out with nullopt");
    expect(elapsed >= 40, "waited (got " + std::to_string(elapsed) + ")");
}

void test_close_wakes_consumers_and_fails_pushes() {
    blocking_queue::BlockingQueue<int> q(4);
    std::atomic<bool> woken{false};
    std::thread t([&] {
        auto v = q.pop(milliseconds(5000));  // would block long...
        woken = true;
        expect(!v.has_value(), "pop after close returns nullopt");
    });
    std::this_thread::sleep_for(milliseconds(30));
    q.close();
    t.join();
    expect(woken.load(), "close woke the blocked pop");
    expect(!q.push(1, milliseconds(10)), "push after close fails");
}

void test_close_drains_remaining() {
    blocking_queue::BlockingQueue<int> q(4);
    q.push(7); q.push(8);
    q.close();
    expect(q.pop().value_or(-1) == 7, "drain 7");
    expect(q.pop().value_or(-1) == 8, "drain 8");
    expect(!q.pop(milliseconds(10)).has_value(), "then nullopt");
}

void test_producer_consumer_throughput() {
    blocking_queue::BlockingQueue<int> q(8);
    constexpr int kItems = 200;
    std::atomic<long> sum{0};

    std::thread producer([&] {
        for (int i = 1; i <= kItems; ++i) q.push(i);
        q.close();
    });
    std::thread consumer([&] {
        while (auto v = q.pop(milliseconds(1000))) sum += *v;
    });
    producer.join();
    consumer.join();
    long expected = static_cast<long>(kItems) * (kItems + 1) / 2;
    expect(sum.load() == expected, "all items consumed exactly once");
}

void test_capacity_never_exceeded() {
    blocking_queue::BlockingQueue<int> q(3);
    q.push(1); q.push(2); q.push(3);
    expect(q.size() == 3, "at capacity");
    expect(q.size() <= q.capacity(), "bounded invariant holds");
}

}  // namespace

int main() {
    try {
        test_fifo_order();
        test_bounded_push_timeout();
        test_pop_timeout_when_empty();
        test_close_wakes_consumers_and_fails_pushes();
        test_close_drains_remaining();
        test_producer_consumer_throughput();
        test_capacity_never_exceeded();
        std::cout << "All blocking_queue tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
