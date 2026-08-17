# Priority Inversion

## 1. Introduction

**Priority inversion** is a scheduling anomaly in real-time and priority-
scheduled systems. A high-priority thread is blocked waiting for a resource held
by a low-priority thread, while a *medium*-priority thread — that does not even
need the resource — runs and preempts the low-priority thread. As a result, the
high-priority thread is indirectly blocked by a medium-priority one.

It is infamous for contributing to the Mars Pathfinder reset bug, where a
high-priority task was delayed by a low-priority one and the watchdog timer
tripped.

## 2. The Problem

Three threads with priorities High > Medium > Low:

```cpp
#include <mutex>
#include <thread>

std::mutex resource;

void low_priority() {
    std::lock_guard<std::mutex> lock(resource);
    do_work();   // holds the lock while doing work
}               // Medium-priority thread can preempt HERE

void medium_priority() {
    // does NOT use resource, but keeps the CPU busy
    busy_loop();
}

void high_priority() {
    std::lock_guard<std::mutex> lock(resource);  // blocks: Low still holds it
    use_resource();
}
```

### Why it's broken

1. Low acquires `resource`.
2. High starts, tries to lock `resource`, and blocks (waits for Low).
3. Medium becomes runnable and preempts Low (Medium > Low in priority), so Low
   cannot finish its critical section.
4. High is now effectively blocked by Medium — the priority order is inverted.

Under a strict priority scheduler, High waits an arbitrarily long time because
Medium keeps running and Low never gets CPU to release the lock.

## 3. The Fix

### 3.1 Priority Inheritance (OS / pthread level)

Most real-time operating systems, and `pthread` mutexes configured with
`PTHREAD_PRIO_INHERIT`, implement **priority inheritance**: while Low holds a
lock that High is waiting on, Low temporarily *inherits* High's priority so it
can finish and release the lock quickly.

```cpp
// POSIX example
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
pthread_mutex_init(&mutex, &attr);
```

### 3.2 Reduce critical-section time

Hold locks for the absolute minimum time; perform heavy computation *outside*
the lock so Low releases `resource` as fast as possible.

### 3.3 Avoid shared locks in priority-sensitive paths

Use lock-free structures or per-thread ownership to remove the shared mutex that
causes the inversion in the first place.

## 4. Best Practices

1. **Use priority-inheriting mutexes** in RTOS / pthread environments.
2. **Keep critical sections tiny** in high-priority threads.
3. **Isolate priority domains** so a medium thread cannot preempt a lock-holder.
4. **Test with simulated load** to surface inversion-induced latency spikes.

## 5. Summary

- Priority inversion: High blocked by Medium because Low holds the lock.
- Caused by priority scheduling + a shared mutex + a preemptive medium thread.
- Fix via priority inheritance, short critical sections, or lock-free design.
- A classic source of mysterious latency in real-time systems.
