// examples/pitfall-use-after-move.cpp
// Demonstrates use-after-move pitfalls and the safe alternatives.
// Compile with: g++ -std=c++17 -o pitfall_use_after_move pitfall-use-after-move.cpp

#include <iostream>
#include <string>
#include <utility>
#include <vector>

// --- 1. Moving from a std::string ---

void movedFromString() {
    std::cout << "--- Moving from a std::string ---\n";

    std::string original = "Hello";
    std::string moved_to = std::move(original);

    // The standard guarantees only "valid but unspecified" for the source.
    std::cout << "  moved_to holds: " << moved_to << "\n";
    std::cout << "  the source is valid but unspecified, this implementation left it "
              << (original.empty() ? "empty" : "non empty")
              << " with size " << original.size() << "\n";

    // BAD: branching on the post move state is not portable.
    // if (original.empty()) { ... }   // the branch may or may not be taken

    // GOOD: reinitialise the source before using it again.
    original = "Hello again";
    std::cout << "  after reassigning the source it holds: " << original << "\n";
}

// --- 2. Moving from a std::vector ---

void movedFromVector() {
    std::cout << "\n--- Moving from a std::vector ---\n";

    std::vector<int> source = {1, 2, 3};
    std::vector<int> sink = std::move(source);

    std::cout << "  the sink holds " << sink.size() << " element(s)\n";
    std::cout << "  the source is valid but unspecified, this implementation left it "
              << (source.empty() ? "empty" : "non empty") << "\n";

    // BAD: the emptiness check is not portable, the branch may be taken on
    // one implementation and skipped on another.
    // if (!source.empty()) { process(source); }
}

// --- 3. A move constructor that leaves the source broken ---

class BadBuffer {
public:
    explicit BadBuffer(std::string data) : buffer_(new std::string(std::move(data))) {
        size_ = buffer_->size();
    }

    BadBuffer(const BadBuffer&) = delete;
    BadBuffer& operator=(const BadBuffer&) = delete;

    BadBuffer(BadBuffer&& other) noexcept : buffer_(other.buffer_), size_(other.size_) {
        // BAD: the buffer is handed over but the size of the source is kept, so
        // the source claims to hold data that it does not own any more.
        other.buffer_ = nullptr;
    }

    ~BadBuffer() { delete buffer_; }

    std::size_t size() const { return size_; }

    // BAD after a move: the source owns no buffer, reading through it would
    // dereference a null pointer. The guard keeps this example free of
    // undefined behaviour.
    const char* data() const { return buffer_ == nullptr ? nullptr : buffer_->c_str(); }

private:
    std::string* buffer_;
    std::size_t size_ = 0;
};

// --- 4. The same class with a consistent moved-from state ---

class GoodBuffer {
public:
    explicit GoodBuffer(std::string data) : buffer_(new std::string(std::move(data))) {
        size_ = buffer_->size();
    }

    GoodBuffer(const GoodBuffer&) = delete;
    GoodBuffer& operator=(const GoodBuffer&) = delete;

    GoodBuffer(GoodBuffer&& other) noexcept : buffer_(other.buffer_), size_(other.size_) {
        // GOOD: the source is left empty and self consistent.
        other.buffer_ = nullptr;
        other.size_ = 0;
    }

    ~GoodBuffer() { delete buffer_; }

    std::size_t size() const { return size_; }

    const char* data() const { return buffer_ == nullptr ? "" : buffer_->c_str(); }

private:
    std::string* buffer_;
    std::size_t size_ = 0;
};

void movedFromBuffer() {
    std::cout << "\n--- A class with a broken moved-from state ---\n";

    BadBuffer source("payload");
    BadBuffer sink(std::move(source));

    // BAD: the source still reports the old size although it owns no buffer.
    // std::cout << source.data() << "\n";   // undefined behaviour
    std::cout << "  the broken source still reports size " << source.size()
              << " although it owns no buffer\n";

    GoodBuffer good_source("payload");
    GoodBuffer good_sink(std::move(good_source));

    std::cout << "  the consistent source reports size " << good_source.size()
              << " and holds \"" << good_source.data() << "\"\n";
    std::cout << "  the destination holds \"" << good_sink.data() << "\"\n";
}

// --- 5. Fix: std::exchange instead of a bare move ---

void exchangeIdiom() {
    std::cout << "\n--- std::exchange instead of a bare move ---\n";

    std::string value = "keep me";
    std::string taken = std::exchange(value, std::string{});

    std::cout << "  taken holds: " << taken << "\n";
    std::cout << "  the source was reset in the same step, it is empty: "
              << (value.empty() ? "yes" : "no") << "\n";
}

// --- 6. Fix: reinitialise the source ---

void reinitialiseAfterMove() {
    std::cout << "\n--- Reinitialising after a move ---\n";

    std::vector<int> numbers = {1, 2, 3};
    std::vector<int> taken = std::move(numbers);

    numbers = {4, 5, 6};  // the source is usable again

    std::cout << "  the destination holds " << taken.size()
              << " element(s), the source holds " << numbers.size() << " element(s) again\n";
}

// --- 7. Fix: guard a move assignment against self assignment ---

class Counter {
public:
    explicit Counter(int value) : value_(value) {}

    Counter(Counter&& other) noexcept : value_(other.value_) { other.value_ = 0; }

    Counter& operator=(Counter&& other) noexcept {
        if (this != &other) {  // a self move keeps the value intact
            value_ = other.value_;
            other.value_ = 0;
        }
        return *this;
    }

    int value() const { return value_; }

private:
    int value_ = 0;
};

void selfMove() {
    std::cout << "\n--- A guarded self move assignment ---\n";

    Counter counter(7);
    Counter& alias = counter;
    alias = std::move(counter);

    std::cout << "  after the guarded self move the value is still " << counter.value() << "\n";
}

int main() {
    movedFromString();
    movedFromVector();
    movedFromBuffer();
    exchangeIdiom();
    reinitialiseAfterMove();
    selfMove();

    std::cout << "\n=== All use after move scenarios completed ===\n";
    return 0;
}
