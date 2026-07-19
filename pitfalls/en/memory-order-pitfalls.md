# Memory Order Pitfalls

## The Problem

Using `std::memory_order_seq_cst` (default) everywhere is safe but slow.
Using relaxed orders incorrectly causes subtle data races.

## Common Mistakes

```cpp
// WRONG: relaxed store/load pair — no happens-before guarantee
std::atomic<int> flag{0};
std::atomic<int> data{0};

// Thread 1
data.store(42, std::memory_order_relaxed);
flag.store(1, std::memory_order_relaxed);

// Thread 2
while (flag.load(std::memory_order_relaxed) == 0) {}
std::cout << data.load(std::memory_order_relaxed);  // may print 0!
```

## Correct Usage

```cpp
// Thread 1: release ensures data write is visible before flag
data.store(42, std::memory_order_release);
flag.store(1, std::memory_order_release);

// Thread 2: acquire ensures we see data after flag
while (flag.load(std::memory_order_acquire) == 0) {}
std::cout << data.load(std::memory_order_acquire);  // guaranteed 42
```

## Rule of Thumb

- `relaxed`: no ordering (counters, statistics)
- `acquire/release`: synchronization between threads
- `seq_cst`: total order (when in doubt, use this)

## Prevention

- Start with `seq_cst`, optimize only after profiling
- Document which variable is the synchronization point
- Use `std::atomic_thread_fence` for complex patterns
- Run with ThreadSanitizer (`-fsanitize=thread`)

## See Also

- [Broken Double-Checked Locking](broken-double-checked-locking.md)
- [Thread Safety Myth](the-myth-of-thread-safety.md)
