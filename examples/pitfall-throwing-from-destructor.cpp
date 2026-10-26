// examples/pitfall-throwing-from-destructor.cpp
// Demonstrates why destructors must be non-throwing and how to report cleanup failures safely.
// Compile with: g++ -std=c++17 -o pitfall_throwing_from_destructor pitfall-throwing-from-destructor.cpp

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

// The error sink that the safe pattern writes its diagnostics to.
struct CleanupStatus {
    int failures{0};
    int unwinding_failures{0};
};

// --- 1. The dangerous pattern ---

class ThrowingResource {
public:
    explicit ThrowingResource(bool fail_on_cleanup) : fail_(fail_on_cleanup) {}

    // noexcept(false) only says "this destructor may throw". It does not make
    // throwing during stack unwinding safe.
    ~ThrowingResource() noexcept(false) {
        if (fail_) {
            throw std::runtime_error("cleanup failed inside the destructor");
        }
    }

private:
    bool fail_;
};

// --- 2. The safe pattern: non-throwing destructor plus an explicit API ---

class Resource {
public:
    Resource(CleanupStatus& status, bool fail_on_close) : status_(status), fail_(fail_on_close) {}

    // GOOD: the destructor is the fallback cleanup path and never throws.
    ~Resource() noexcept {
        const bool unwinding = std::uncaught_exceptions() > 0;
        try {
            close_impl();
        } catch (...) {
            // Swallow the failure and report it out of band.
            ++status_.failures;
            if (unwinding) {
                ++status_.unwinding_failures;
            }
        }
    }

    // GOOD: the primary error reporting path, it may throw.
    void close() { close_impl(); }

private:
    void close_impl() {
        if (fail_) {
            throw std::runtime_error("close failed");
        }
        closed_ = true;
    }

    CleanupStatus& status_;
    bool fail_;
    bool closed_{false};
};

}  // namespace

// --- 1. A destructor that throws during stack unwinding ---

void terminateDuringUnwinding() {
    std::cout << "--- A destructor that throws during stack unwinding ---\n";

    // BAD: the destructor throws while another exception is already
    // propagating. The runtime cannot merge two active exception flows, so
    // it calls std::terminate() and the process aborts. It is not executed
    // here.
    // try {
    //     ThrowingResource guard{true};
    //     throw std::runtime_error("primary failure");
    // } catch (const std::exception& e) {
    //     std::cout << "caught: " << e.what() << "\n";
    // }
    // Unwinding runs ~ThrowingResource, which throws a second exception:
    // std::terminate() -> abort, the catch handler above is never reached.

    std::cout << "The aborting case is not executed, see the comment above.\n";
}

// --- 2. The same destructor outside stack unwinding ---

void throwOutsideUnwinding() {
    std::cout << "\n--- The same destructor outside stack unwinding ---\n";

    try {
        ThrowingResource guard{true};
        std::cout << "scope body finished\n";
    } catch (const std::exception& e) {
        std::cout << "caught from the destructor: " << e.what() << "\n";
    }

    std::cout << "no unwinding was in progress, so the program survives\n";
}

// --- 3. The safe pattern ---

void safePattern() {
    std::cout << "\n--- The safe pattern: noexcept destructor, explicit close() ---\n";

    CleanupStatus status;

    {
        Resource resource(status, /*fail_on_close=*/true);
        try {
            resource.close();  // the explicit API reports the failure
        } catch (const std::exception& e) {
            std::cout << "close() reported: " << e.what() << "\n";
        }
    }  // ~Resource runs, catches the failure and records it

    std::cout << "cleanup failures recorded:  " << status.failures << "\n";
    std::cout << "failures during unwinding:  " << status.unwinding_failures << "\n";

    CleanupStatus clean_status;
    {
        Resource resource(clean_status, /*fail_on_close=*/false);
        resource.close();
        std::cout << "close() on a healthy resource succeeded\n";
    }
    std::cout << "cleanup failures recorded:  " << clean_status.failures << "\n";
}

int main() {
    terminateDuringUnwinding();
    throwOutsideUnwinding();
    safePattern();

    std::cout << "\n=== All throwing destructor scenarios completed ===\n";
    return 0;
}
