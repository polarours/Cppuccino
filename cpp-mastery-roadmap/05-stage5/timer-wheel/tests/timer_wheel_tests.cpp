#include "timer_wheel.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_fire_at_exact_tick() {
    timer_wheel::TimerWheel w(8, 1);
    int fired = 0;
    w.scheduleIn(3, [&] { ++fired; });
    w.tick(2);
    expect(fired == 0, "not fired before due tick");
    w.tick(1);  // now at tick 3
    expect(fired == 1, "fired at due tick");
    expect(w.pendingCount() == 0, "bucket drained");
}

void test_past_schedule_fires_next_tick() {
    timer_wheel::TimerWheel w(8, 1);
    int fired = 0;
    w.scheduleAt(0, [&] { ++fired; });  // already due
    w.tick();
    expect(fired == 1, "past-due fires immediately on tick");
}

void test_cancel_prevents_fire() {
    timer_wheel::TimerWheel w(8, 1);
    int fired = 0;
    auto id = w.scheduleIn(2, [&] { ++fired; });
    expect(w.cancel(id), "cancel returns true");
    expect(!w.cancel(id), "second cancel false");
    w.tick(10);
    expect(fired == 0, "cancelled timer never fires");
    expect(w.pendingCount() == 0, "no pending left");
}

void test_multiple_slots_full_rotation() {
    // slots=4: timers at +5 must fire after a full rotation, not at +1
    timer_wheel::TimerWheel w(4, 1);
    int fired = 0;
    w.scheduleIn(5, [&] { ++fired; });
    w.tick(4);   // absolute fire tick = 5; at tick 4 not yet due
    expect(fired == 0, "not fired early across rotation");
    w.tick(1);   // tick = 5
    expect(fired == 1, "fired exactly at abs tick 5");
}

void test_same_slot_different_times() {
    timer_wheel::TimerWheel w(4, 1);
    int a = 0, b = 0;
    w.scheduleIn(4, [&] { ++a; });  // abs 4 -> same slot as abs 0
    w.scheduleIn(8, [&] { ++b; });  // abs 8 -> same slot again
    w.tick(4);  // tick 4: bucket for slot 0 has entry abs=4? fire check <= tick
    expect(a == 1, "a fired at tick 4");
    expect(b == 0, "b (abs 8) rescheduled into wheel");
    w.tick(4);  // tick 8
    expect(b == 1, "b fired at tick 8");
}

void test_cancel_all_then_tick_idempotent() {
    timer_wheel::TimerWheel w(8, 1);
    int fired = 0;
    for (int i = 1; i <= 3; ++i) w.scheduleIn(i, [&] { ++fired; });
    w.tick(10);
    expect(fired == 3, "all three fired");
    w.tick(10);
    expect(fired == 3, "no double fire");
}

void test_interval_reported() {
    timer_wheel::TimerWheel w(8, 25);
    expect(w.intervalMs() == 25, "interval stored");
}

}  // namespace

int main() {
    try {
        test_fire_at_exact_tick();
        test_past_schedule_fires_next_tick();
        test_cancel_prevents_fire();
        test_multiple_slots_full_rotation();
        test_same_slot_different_times();
        test_cancel_all_then_tick_idempotent();
        test_interval_reported();
        std::cout << "All timer_wheel tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
