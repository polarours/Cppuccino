// examples/pitfall-memory-leak.cpp
// Demonstrates memory leaks caused by manual ownership and the RAII fix.
// Compile with: g++ -std=c++17 -o pitfall_memory_leak pitfall-memory-leak.cpp

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// The destructor prints, so the program shows *when* the memory is released.
struct Record {
    Record(std::string recordName, int recordValue)
        : name(std::move(recordName)), value(recordValue) {}
    ~Record() { std::cout << "    Record '" << name << "' destroyed\n"; }

    Record(const Record&) = default;
    Record& operator=(const Record&) = default;
    Record(Record&&) = default;
    Record& operator=(Record&&) = default;

    std::string name;
    int value;
};

// --- 1. The leak: a `new` without a matching `delete` on every path ---

void leakyProcess(bool skip) {
    // BAD: ownership is manual, so it only works on the happy path.
    // Record* r = new Record{"alpha", 1};   // (1) ownership begins here
    // if (skip) {
    //     return;                            // (2) LEAK: r is never deleted
    // }
    // delete r;                             // (3) only reached on one path
    //
    // Under the sanitizers this would report:
    //   ERROR: LeakSanitizer: detected memory leaks
    // The leaky body stays commented out so that this program is leak-free
    // and can be run under -fsanitize=address.

    // GOOD: the owner is tied to the scope, so every exit path frees.
    auto r = std::make_unique<Record>("alpha", 1);
    if (skip) {
        std::cout << "  early return taken -- the owner still frees the record\n";
        return;
    }
    std::cout << "  happy path -- the owner frees the record at scope exit\n";
}

// --- 2. The exception path ---

void throwingProcess(bool shouldThrow) {
    // BAD: the exception leaves the function before `delete` is reached.
    // Record* r = new Record{"beta", 2};
    // if (shouldThrow) { throw std::runtime_error("work failed"); }
    // delete r;

    // GOOD: during stack unwinding the owner is destroyed automatically.
    auto r = std::make_unique<Record>("beta", 2);
    std::cout << "  acquired the record\n";
    if (shouldThrow) {
        throw std::runtime_error("work failed");
    }
    std::cout << "  finished the work\n";
}

// --- 3. Containers own their elements ---

void containersOwnTheirElements() {
    std::cout << "\n--- 3. Let the container own the elements ---\n";

    // BAD: the vector stores raw pointers, so it frees nothing.
    // std::vector<Record*> raw;
    // raw.push_back(new Record{"gamma", 3});   // nobody ever deletes these
    // std::cout << "  raw.size() = " << raw.size() << "\n";

    // GOOD: the vector owns the records and frees them with itself.
    std::vector<Record> owned;
    owned.reserve(2);  // reserve up front, so the records are never moved around
    owned.emplace_back("gamma", 3);
    owned.emplace_back("delta", 4);
    std::cout << "  owned.size() = " << owned.size() << "\n";
}

// --- 4. RAII for resources that are not memory ---

class FileHandle {
public:
    FileHandle() : file_(std::tmpfile()) {}
    ~FileHandle() {
        if (file_ != nullptr) {
            std::fclose(file_);
        }
    }

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    bool valid() const { return file_ != nullptr; }

private:
    std::FILE* file_;
};

void raiiForFileHandles() {
    std::cout << "\n--- 4. RAII for a file handle ---\n";

    // BAD: the handle leaks if the function returns early.
    // std::FILE* file = std::tmpfile();
    // if (earlyExit) { return; }            // the handle is never closed
    // std::fclose(file);

    // GOOD: the wrapper closes the handle on every exit path.
    FileHandle handle;
    std::cout << "  handle opened: " << (handle.valid() ? "yes" : "no") << "\n";
    std::cout << "  handle closed by the destructor at scope exit\n";
}

int main() {
    std::cout << "--- 1. Manual ownership leaks on the other paths ---\n";
    leakyProcess(true);
    leakyProcess(false);

    std::cout << "\n--- 2. The exception path ---\n";
    try {
        throwingProcess(true);
    } catch (const std::exception& e) {
        std::cout << "  caught: " << e.what() << "\n";
    }

    containersOwnTheirElements();
    raiiForFileHandles();

    std::cout << "\n=== All memory leak scenarios completed ===\n";
    return 0;
}
