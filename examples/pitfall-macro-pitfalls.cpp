// examples/pitfall-macro-pitfalls.cpp
// Demonstrates macro pitfalls (precedence, double evaluation, statement macros) and their fixes.
// Compile with: g++ -std=c++17 -o pitfall_macro_pitfalls pitfall-macro-pitfalls.cpp

#include <algorithm>
#include <iostream>
#include <string>

// --- 1. Operator precedence ---

#define SQUARE_UNSAFE(x) x * x
#define SQUARE_SAFE(x) ((x) * (x))

void operatorPrecedence() {
    std::cout << "--- 1. Operator precedence ---\n";

    int a = 3;
    // SQUARE_UNSAFE(a + 1) expands to: a + 1 * a + 1  ==  3 + 3 + 1  ==  7
    int unsafeResult = SQUARE_UNSAFE(a + 1);
    // SQUARE_SAFE(a + 1) expands to:   ((a + 1) * (a + 1))  ==  16
    int safeResult = SQUARE_SAFE(a + 1);

    std::cout << "  SQUARE_UNSAFE(a + 1) = " << unsafeResult << " (expected 16)\n";
    std::cout << "  SQUARE_SAFE(a + 1)   = " << safeResult << " (expected 16)\n";

    // The macro is also not type safe: SQUARE_SAFE("abc") would not compile.
}

// --- 2. Multiple evaluation of the arguments ---

#define MAX_UNSAFE(a, b) ((a) > (b) ? (a) : (b))

template <typename T>
constexpr T maxSafe(const T& left, const T& right) {
    return left > right ? left : right;
}

int bumpCalls = 0;

int bump() {
    ++bumpCalls;
    return bumpCalls;
}

void multipleEvaluation() {
    std::cout << "\n--- 2. The arguments are evaluated more than once ---\n";

    bumpCalls = 0;
    // MAX_UNSAFE(bump(), 0) expands to: ((bump()) > (0) ? (bump()) : (0))
    // so bump() runs twice and the result is 2, not 1.
    int unsafeResult = MAX_UNSAFE(bump(), 0);
    std::cout << "  MAX_UNSAFE(bump(), 0) = " << unsafeResult
              << ", bump() called " << bumpCalls << " times (expected 1 call)\n";

    bumpCalls = 0;
    int safeResult = maxSafe(bump(), 0);
    std::cout << "  maxSafe(bump(), 0)   = " << safeResult
              << ", bump() called " << bumpCalls << " times\n";
}

// --- 3. A macro that expands to several statements ---

#define STEP_UNSAFE(x) x++; x++
#define STEP_SAFE(x) do { (x)++; } while (0)

void statementMacro() {
    std::cout << "\n--- 3. Multi-statement macros break the surrounding control flow ---\n";

    int unsafeCounter = 0;
    // The compiler detects this shape with -Wmultistatement-macros. The warning
    // is exactly the lesson of this section, so it is silenced for the two
    // deliberate uses below.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmultistatement-macros"
    if (unsafeCounter == 0)
        STEP_UNSAFE(unsafeCounter);  // expands to: unsafeCounter++; unsafeCounter++;
#pragma GCC diagnostic pop
    // The second increment is NOT guarded by the if, so it always runs.
    std::cout << "  STEP_UNSAFE inside an if: counter = " << unsafeCounter << " (expected 1)\n";

    // The same macro inside an if/else chain does not even compile, because the
    // `else` no longer belongs to any `if`:
    //   if (cond) STEP_UNSAFE(counter); else counter = -1;   // ERROR: else without if

    int safeCounter = 0;
    if (safeCounter == 0)
        STEP_SAFE(safeCounter);  // do { ... } while (0) is a single statement
    std::cout << "  STEP_SAFE inside an if:   counter = " << safeCounter << " (expected 1)\n";
}

// --- 4. Commas inside macro arguments ---

#define ADD_UNSAFE(a, b) ((a) + (b))
#define ADD_VARIADIC(...) addAll(__VA_ARGS__)

int addAll(int left, int right) { return left + right; }

void commaInArguments() {
    std::cout << "\n--- 4. A comma inside an argument splits the argument list ---\n";

    // BAD: the comma inside std::max(1, 2) is a macro argument separator, so
    // ADD_UNSAFE sees three arguments and fails to compile.
    //   int broken = ADD_UNSAFE(std::max(1, 2), 3);   // ERROR: macro passed 3 arguments

    // GOOD: wrap the argument in parentheses so the comma is hidden.
    int fixed = ADD_UNSAFE((std::max(1, 2)), 3);
    std::cout << "  ADD_UNSAFE((std::max(1, 2)), 3) = " << fixed << "\n";

    // A variadic macro forwards the whole argument list instead of splitting it.
    int variadic = ADD_VARIADIC(std::max(1, 2), 3);
    std::cout << "  ADD_VARIADIC(std::max(1, 2), 3) = " << variadic << "\n";
}

// --- 5. Macros are text substitution, not function calls ---

#define LOG_UNSAFE(msg) std::cout << msg << "\n"
#define LOG_SAFE(msg) std::cout << std::string(msg) << "\n"

void textSubstitution() {
    std::cout << "\n--- 5. The macro argument is pasted into the statement ---\n";

    const std::string name = "World";

    // BAD: "Hello " + name is not valid, the + operator does not apply to const char*.
    //   LOG_UNSAFE("Hello " + name);   // ERROR: invalid operands to binary +

    // GOOD: the argument is turned into a std::string first.
    LOG_SAFE("Hello " + name);

    // LOG_UNSAFE(Hello World) would be pasted as two undeclared identifiers.
}

// --- 6. The constexpr replacement and macro hygiene ---

#define CPPUCCINO_SQUARE(x) ((x) * (x))

constexpr int squareConstexpr(int value) { return value * value; }

void hygiene() {
    std::cout << "\n--- 6. Prefer constexpr functions and keep macros local ---\n";

    std::cout << "  CPPUCCINO_SQUARE(5)   = " << CPPUCCINO_SQUARE(5) << "\n";
    std::cout << "  squareConstexpr(5)   = " << squareConstexpr(5) << "\n";

    // A macro does not respect namespaces or scopes: it is visible everywhere
    // after its definition until it is undefined again.
#undef CPPUCCINO_SQUARE
    // Using CPPUCCINO_SQUARE(5) after the #undef would fail to compile.
}

int main() {
    operatorPrecedence();
    multipleEvaluation();
    statementMacro();
    commaInArguments();
    textSubstitution();
    hygiene();

    std::cout << "\n=== All macro pitfall scenarios completed ===\n";
    return 0;
}
