// examples/pitfall-narrowing-conversions.cpp
// Demonstrates narrowing conversions in brace, parenthesis and copy initialization.
// Compile with: g++ -std=c++17 -o pitfall_narrowing_conversions pitfall-narrowing-conversions.cpp

#include <cstdint>
#include <iostream>
#include <string>

// --- 1. Brace initialization rejects narrowing at compile time ---

void braceInitialization() {
    std::cout << "--- Brace initialization ---\n";

    // GOOD: the value fits, so the brace initialization is accepted.
    int exact{42};
    std::cout << "int exact{42} = " << exact << "\n";

    // BAD: these are narrowing conversions, so the compiler rejects them.
    // int truncated{3.14};   // ERROR: narrowing conversion from 'double'
    // char letter{300};     // ERROR: narrowing conversion from 'int'
    // long long wide{value};// ERROR when value does not fit in the target

    // GOOD: a narrowing conversion that is intended has to be written down.
    int truncated{static_cast<int>(3.14)};
    std::cout << "int truncated{static_cast<int>(3.14)} = " << truncated << "\n";
}

// --- 2. Parenthesized and copy initialization truncate silently ---

void silentTruncation() {
    std::cout << "\n--- Parenthesized and copy initialization ---\n";

    // BAD: no diagnostic, the fractional part is dropped.
    int from_paren(3.99);
    int from_copy = 3.99;
    std::cout << "int from_paren(3.99) = " << from_paren << "\n";
    std::cout << "int from_copy = 3.99  = " << from_copy << "\n";

    // BAD: a long long silently becomes a char.
    long long big = 300;
    char letter = big;
    std::cout << "char letter = big (300) = " << static_cast<int>(letter) << "\n";

    // GOOD: the intent is visible in the source.
    char explicit_letter = static_cast<char>(big);
    std::cout << "char static_cast<char>(300) = " << static_cast<int>(explicit_letter) << "\n";
}

// --- 3. Integer wraparound in unsigned narrow types ---

void unsignedWraparound() {
    std::cout << "\n--- Unsigned narrow types wrap around ---\n";

    // BAD: 256 does not fit in 8 bits, the value wraps to 0.
    // A literal (std::uint8_t a = 256;) also triggers -Woverflow on modern
    // compilers, so a variable is used here to show the silent wraparound.
    int value = 256;
    std::uint8_t a = value;
    std::cout << "uint8_t a = 256 (via a variable) -> " << static_cast<int>(a) << "\n";

    // The same wrap happens with an explicit cast, it is just visible.
    std::uint8_t b = static_cast<std::uint8_t>(256);
    std::cout << "uint8_t(256)             -> " << static_cast<int>(b) << "\n";

    // BAD: a negative value becomes a large unsigned value.
    std::int16_t negative = -1;
    std::uint16_t positive = static_cast<std::uint16_t>(negative);
    std::cout << "uint16_t(int16_t(-1))    -> " << positive << "\n";

    // GOOD: keep the value in a wide type when the range is not known.
    std::int64_t wide = negative;
    std::cout << "int64_t kept as is       -> " << wide << "\n";
}

// --- 4. A range checked conversion helper ---

// Returns false instead of truncating when the value does not fit.
bool checked_int_to_char(long long value, char& out) {
    if (value < 0 || value > 255) {
        return false;
    }
    out = static_cast<char>(value);
    return true;
}

void checkedConversion() {
    std::cout << "\n--- Checked conversion ---\n";

    char out{0};
    if (checked_int_to_char(65, out)) {
        std::cout << "65 fits in a char -> " << static_cast<int>(out) << "\n";
    }
    if (!checked_int_to_char(300, out)) {
        std::cout << "300 does not fit in a char -> rejected\n";
    }
}

int main() {
    braceInitialization();
    silentTruncation();
    unsignedWraparound();
    checkedConversion();

    std::cout << "\n=== All narrowing conversion scenarios completed ===\n";
    return 0;
}
