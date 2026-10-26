// examples/pitfall-fold-expression-errors.cpp
// Demonstrates fold expression mistakes: empty packs, wrong parentheses, and the fixes.
// Compile with: g++ -std=c++17 -o pitfall_fold_expression_errors pitfall-fold-expression-errors.cpp

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// --- 1. Empty pack with an operator that has no identity element ---

// BAD: for `+` the standard requires at least one element, so sumBad() is
// ill-formed ("no match for operator+"). The template itself compiles, but
// instantiating it with an empty pack does not.
// template <typename... Ts>
// auto sumBad(Ts... ts) {
//     return (ts + ...);   // ERROR when called as sumBad(): empty pack
// }

// GOOD: a binary fold with an initializer is well-formed for an empty pack.
template <typename... Ts>
auto sum(Ts... ts) {
    return (ts + ... + 0);
}

// The same trick works for multiplication (init 1) and bitwise or (init 0).
template <typename... Ts>
auto product(Ts... ts) {
    return (ts * ... * 1);
}

// `&&` and `||` already have a defined value for an empty pack, but an
// explicit initializer keeps the intent obvious.
template <typename... Ts>
bool allOf(Ts... ts) {
    return (ts && ... && true);
}

template <typename... Ts>
bool anyOf(Ts... ts) {
    return (ts || ... || false);
}

// --- 2. Wrong parentheses: the pack must sit inside the fold ---

template <typename... Ts>
void printAll(std::ostream& os, Ts... ts) {
    // GOOD: unary left fold, the whole fold is wrapped in parentheses.
    (os << ... << ts);
}

// --- 3. Order matters for non-associative operators ---

template <typename... Ts>
auto subtractLeft(Ts... ts) {
    return (... - ts);  // left fold:  ((a - b) - c)
}

template <typename... Ts>
auto subtractRight(Ts... ts) {
    return (ts - ...);  // right fold: (a - (b - c))
}

// --- 4. Comma fold for side effects ---

template <typename... Ts>
void printEach(Ts... items) {
    // The comma operator evaluates every element, in order.
    ((std::cout << "[item] " << items << '\n'), ...);
}

int main() {
    std::cout << "--- Empty pack needs an initializer ---\n";
    // BAD (not executed, it does not compile):
    // std::cout << "sum of nothing: " << sumBad() << "\n";
    std::cout << "sum()          = " << sum() << "\n";
    std::cout << "sum(1, 2, 3)  = " << sum(1, 2, 3) << "\n";
    std::cout << "product()      = " << product() << "\n";
    std::cout << "product(2, 3)  = " << product(2, 3) << "\n";
    std::cout << "allOf(true)    = " << allOf(true) << "\n";
    std::cout << "allOf()        = " << allOf() << "\n";
    std::cout << "anyOf()        = " << anyOf() << "\n";

    std::cout << "\n--- Fold over a pack of strings ---\n";
    std::ostringstream out;
    printAll(out, "a", "b", "c");
    std::cout << "folded output: " << out.str() << "\n";

    // BAD (not executed, it is a parse error):
    // std::ostringstream bad;
    // bad << (... << "a");   // ERROR: expected unqualified-id, the pack is
    //                        // missing so the compiler sees no fold

    std::cout << "\n--- Fold direction changes the result ---\n";
    std::cout << "left fold  (a - b) - c = " << subtractLeft(10, 3, 2) << "\n";
    std::cout << "right fold a - (b - c) = " << subtractRight(10, 3, 2) << "\n";

    std::cout << "\n--- Comma fold runs every element ---\n";
    printEach("first", "second", "third");

    std::cout << "\n=== All fold expression scenarios completed ===\n";
    return 0;
}
