// examples/pitfall-implicit-conversions.cpp
// Demonstrates implicit conversions in C++ and the bugs they silently cause.
// Compile with: g++ -std=c++17 -o pitfall_implicit_conversions pitfall-implicit-conversions.cpp

#include <cmath>
#include <iostream>
#include <string>
#include <type_traits>

// --- 1. Standard conversion sequences ---

void standardConversions() {
    std::cout << "--- 1. Standard conversions ---\n";

    int i = 10;
    double d = 5.5;
    auto sum = i + d;  // `i` is converted to double
    static_assert(std::is_same<decltype(sum), double>::value, "int + double is a double");
    std::cout << "  int + double -> double : 10 + 5.5 = " << sum << "\n";

    char c = 'A';
    auto promoted = c + 1;  // `char` is promoted to int
    static_assert(std::is_same<decltype(promoted), int>::value, "char + int is an int");
    std::cout << "  char + int  -> int    : 'A' + 1 = " << promoted << "\n";

    double pi = 3.99;
    int truncated = pi;  // floating point -> integer truncates the fraction
    std::cout << "  double -> int         : 3.99 becomes " << truncated << " (truncated)\n";
    std::cout << "  explicit rounding     : static_cast<int>(std::round(3.99)) = "
              << static_cast<int>(std::round(pi)) << "\n";
}

// --- 2. Signed / unsigned mixing ---

void signedUnsignedMixing() {
    std::cout << "\n--- 2. Signed / unsigned mixing ---\n";

    int a = -1;
    unsigned int b = 1;

    // The natural spelling of the comparison is what -Wsign-compare warns
    // about, so the conversion that actually happens is spelled out here.
    std::cout << "  -1 converted to unsigned int = " << static_cast<unsigned int>(a) << "\n";

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
    if (a < b) {
        std::cout << "  a < b : true\n";
    } else {
        std::cout << "  a < b : false  (a was converted to " << static_cast<unsigned int>(a) << ")\n";
    }
#pragma GCC diagnostic pop
}

// --- 3. Boolean conversion: an assignment used as a condition ---

void booleanConversion() {
    std::cout << "\n--- 3. Assignment used as a condition ---\n";

    int x = 0;
    int y = 42;

    // BAD: `x = y` assigns, and the result is then converted to bool.
    // It compiles, but it almost always means `x == y` was intended.
    // if (x = y) { std::cout << "assigned\n"; }

    // GOOD: compare explicitly ...
    if (x == y) {
        std::cout << "  x == y\n";
    } else {
        std::cout << "  x != y\n";
    }

    // ... or, if an assignment really is intended, make it obvious with an
    // extra pair of parentheses (which also silences -Wparentheses).
    if ((x = y)) {
        std::cout << "  x was assigned, new value = " << x << "\n";
    }
}

// --- 4. A user-defined implicit conversion ---

class FilePath {
public:
    FilePath(const char* path) : path_(path) {}  // implicit on purpose
    const std::string& value() const { return path_; }

private:
    std::string path_;
};

void openFile(FilePath path) {
    std::cout << "  opening " << path.value() << "\n";
}

// --- 5. `explicit` restores the type boundary ---

class UserId {
public:
    explicit UserId(int id) : id_(id) {}
    int value() const { return id_; }

private:
    int id_;
};

// --- 6. A conversion operator erases the distinction again ---

class Meters {
public:
    explicit Meters(int value) : value_(value) {}
    operator int() const { return value_; }  // implicit conversion back to int
    int value() const { return value_; }

private:
    int value_;
};

// --- 7. Distinct types for distinct meanings ---

struct Width {
    int value;
};

struct Height {
    int value;
};

void resize(Width w, Height h) {
    std::cout << "  resizing to " << w.value << "x" << h.value << "\n";
}

int main() {
    standardConversions();
    signedUnsignedMixing();
    booleanConversion();

    std::cout << "\n--- 4. Implicit user-defined conversion ---\n";
    openFile("config.json");  // const char* is silently converted to FilePath

    std::cout << "\n--- 5. explicit keeps the call site honest ---\n";
    UserId uid(42);  // OK
    // UserId uid2 = 42;  // compile error: the implicit conversion is disabled
    std::cout << "  UserId value = " << uid.value() << "\n";

    std::cout << "\n--- 6. A conversion operator erases the type ---\n";
    Meters distance(1500);
    int raw = distance;  // Meters silently becomes an int
    std::cout << "  Meters " << distance.value() << " became the int " << raw << "\n";

    std::cout << "\n--- 7. Distinct types forbid the mistake ---\n";
    resize(Width{1920}, Height{1080});
    // resize(1920, 1080);  // compile error: int does not convert to Width/Height

    std::cout << "\n=== All implicit conversion scenarios completed ===\n";
    return 0;
}
