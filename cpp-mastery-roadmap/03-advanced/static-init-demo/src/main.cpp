#include <iostream>
#include "counter.hpp"

// Bad demo: global objects in different translation units
extern void demonstrateBad();

// Good demo: using function-scope static variables (Meyers' singleton)
// which solve the static initialization order problem

namespace {
Counter& getGoodCounter() {
    static Counter good("Good");  // Constructed on first use
    return good;
}
}

void demonstrateGood() {
    std::cout << "\n--- Good Demo (function-scope static) ---\n";
    std::cout << "No static init order issue - initialization happens on first use.\n";
}

int main() {
    demonstrateBad();
    demonstrateGood();
    return 0;
}
