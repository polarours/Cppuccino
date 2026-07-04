# Throwing from a Destructor

## 0. Scope and Immediate Conclusion

This chapter discusses a classic C++ exception-safety pitfall:

- a destructor emits an exception,
- stack unwinding is already in progress,
- program is terminated via `std::terminate`.

Immediate conclusion:

1. Destructors should be non-throwing by design.
2. Resource release failures must be handled without propagating exceptions from destructors.
3. If cleanup can fail meaningfully, expose an explicit operation (`close()`, `commit()`, `flush()`) and handle errors there.

---

## 1. Minimal Example

```cpp
struct X {
    ~X() noexcept(false) {
        throw 1;
    }
};
```

This destructor is explicitly declared as potentially throwing.

If `~X()` runs while another exception is propagating, the runtime cannot continue normal exception handling and will call `std::terminate`.

---

## 2. Why Termination Happens

During exception propagation, the runtime performs stack unwinding:

1. objects with automatic storage are destroyed,
2. destructors execute as cleanup actions,
3. control transfers toward a matching handler.

If a second exception escapes a destructor during this process, there are now competing active exception flows.

C++ resolves this by termination.

Operationally:

- normal recovery path is abandoned,
- terminate handler is invoked,
- process exits unless custom handler changes behavior (it still cannot resume normal execution safely).

---

## 3. Language and Library Design Intent

RAII relies on deterministic cleanup.

For RAII to remain reliable in exceptional flows:

- cleanup must not introduce new escaping failures,
- destructors must behave as “best-effort finalization without throw”.

This design keeps exception propagation single-threaded in semantic space:

$$
	ext{One active failure path at a time}
$$

When destructors throw, this invariant breaks.

---

## 4. Common Misconceptions

### 4.1 “I can mark destructor `noexcept(false)` so it is fine”

No. It only permits throwing in type signature terms.

It does not make dual-exception unwinding safe.

### 4.2 “It throws only rarely” 

Rare failure modes are exactly the ones that hit production under stress.

Termination under stress is often worse than graceful degradation.

### 4.3 “I need to report cleanup failure, so throwing is best”

In destructors, reporting should be out-of-band:

- logging,
- telemetry,
- error counters,
- explicit pre-destruction API.

---

## 5. High-Risk Real-World Scenarios

### 5.1 File wrapper flushing in destructor

```cpp
class FileWriter {
public:
    ~FileWriter() {
        // bad idea: throw on flush failure
    }
};
```

Disk full / I/O errors during unwinding can terminate process.

### 5.2 Transaction object auto-rollback throwing

Rollback failure inside destructor during another exception path may terminate critical services.

### 5.3 Network connection teardown exceptions

Socket close/shutdown error should not escape destructor.

### 5.4 Logging backends that throw in destructor

Diagnostic code causing termination during fatal-path handling is especially harmful.

---

## 6. Correct Pattern A: Destructors Are `noexcept`

Design principle:

- `~T()` must not emit exceptions.

Example:

```cpp
class Socket {
public:
    ~Socket() noexcept {
        try {
            close_impl();
        } catch (...) {
            record_close_failure();
        }
    }

    void close() {
        close_impl(); // explicit API may throw if contract allows
    }

private:
    void close_impl();
    void record_close_failure() noexcept;
};
```

Interpretation:

- destructor is fallback cleanup path,
- explicit `close()` is primary error-reporting path.

---

## 7. Correct Pattern B: Two-Phase Resource Protocol

For resources with meaningful failure semantics, separate phases:

1. **finalize/commit/close**: explicit, may fail and report.
2. **destructor**: non-throwing safety net.

Example model:

```cpp
class OutputFile {
public:
    void close();              // may report errors
    ~OutputFile() noexcept;    // never throws
};
```

This pattern is often the cleanest compromise between correctness and observability.

---

## 8. Correct Pattern C: Error Sinks Instead of Throwing

When destructor cleanup fails:

- append to diagnostics buffer,
- increment a metric,
- emit structured logs,
- set a status in externally owned state object.

Example shape:

```cpp
struct CleanupStatus {
    std::atomic<int> failures{0};
};

class Guard {
public:
    Guard(CleanupStatus& s) : status_(s) {}
    ~Guard() noexcept {
        if (!cleanup_ok()) {
            status_.failures.fetch_add(1, std::memory_order_relaxed);
        }
    }
private:
    bool cleanup_ok() noexcept;
    CleanupStatus& status_;
};
```

---

## 9. Explicitly Calling Destructors: Extra Danger

Manual destructor invocation is already niche and risky.

If such a destructor throws, lifetime and control-flow logic become even harder to reason about.

Guideline:

- avoid manual destructor calls except in carefully controlled low-level code,
- never rely on thrown exceptions from destructor in that context.

---

## 10. Interaction with Move Operations and Containers

Containers and algorithms prefer strong exception assumptions for element destruction and movement.

Throwing destructors can:

- break exception guarantees,
- force pessimistic behavior,
- complicate generic code assumptions.

Even if code “seems to work,” type-level exception behavior can degrade library guarantees.

---

## 11. Domain-Specific Notes

### 11.1 Database and transactions

Do not hide commit failure in destructor.

- require explicit `commit()` result handling,
- destructor performs rollback/best-effort cleanup without throw.

### 11.2 Files and streams

Flushing errors should be observed before object lifetime ends via explicit APIs.

### 11.3 Distributed systems

Destructor throw leading to termination may amplify incident impact across nodes.

Prefer fault containment and reporting.

---

## 12. Testing Strategy

### 12.1 Fault injection

Inject cleanup failures in:

- file close,
- socket close,
- transaction rollback.

Verify:

1. no exception escapes destructor,
2. process does not terminate unexpectedly,
3. diagnostics are emitted.

### 12.2 Unwinding-path tests

Force a primary exception, then trigger destructor cleanup failure.

Expected behavior:

- cleanup failure captured internally,
- original exception handling path preserved.

### 12.3 Contract tests

Test explicit `close()/commit()` behavior independently from destructor fallback.

---

## 13. Static and Dynamic Analysis

Recommended checks:

1. lint/static rules flagging throwing destructors.
2. code search for `~Type() noexcept(false)`.
3. review of destructor bodies that call potentially throwing APIs.

Dynamic analysis cannot prove absence of throw paths alone; pair with code review.

---

## 14. Code Review Checklist

When reviewing a destructor:

1. Is it explicitly or implicitly non-throwing?
2. Does body call functions that may throw?
3. Are thrown errors converted to diagnostics instead of escaping?
4. Is there an explicit API for failure-reporting operations?
5. Are invariants still preserved if cleanup partially fails?

Red flags:

- `~T() noexcept(false)`,
- `throw` statement in destructor,
- destructor invoking I/O or commit-like operations without catch boundary.

---

## 15. Migration Plan for Legacy Code

If existing code has throwing destructors:

1. Inventory all such types.
2. Classify operations performed in destructors (I/O, network, transaction, locks).
3. Introduce explicit error-reporting APIs.
4. Wrap destructor cleanup with catch-all and telemetry.
5. Deprecate dangerous destructor behavior and migrate call sites.

Roll out incrementally with regression tests and fault injection.

---

## 16. Design Rule Summary

Use this rule hierarchy:

1. Destructor = non-throwing finalizer.
2. Explicit method = place for recoverable failure reporting.
3. Logging/metrics = side channel for destructor cleanup anomalies.

Mathematical shorthand:

$$
	ext{RAII cleanup reliability} > \text{attempting exception transport from destructor}
$$

---

## 17. Bad vs Good Patterns

| Intent | Fragile Pattern | Robust Pattern |
|---|---|---|
| report close failure | throw in `~T()` | explicit `close()` return/throw + non-throwing destructor |
| fallback cleanup | potentially throwing API call in destructor | catch-all in destructor + telemetry |
| transaction finalization | implicit commit in destructor that may throw | explicit `commit()` and non-throwing rollback in destructor |

---

## 18. Key Takeaways

1. Throwing from destructors is a severe exception-safety hazard.
2. If it happens during stack unwinding, `std::terminate` is likely outcome.
3. Keep destructors non-throwing and predictable.
4. Move meaningful failure reporting to explicit APIs.
5. Use diagnostics, not propagated exceptions, in destructor cleanup paths.

Reliable cleanup semantics are foundational for production C++ systems.
