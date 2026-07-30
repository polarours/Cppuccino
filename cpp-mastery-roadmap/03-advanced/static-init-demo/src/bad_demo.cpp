#include "counter.hpp"
#include <iostream>

// Global instances in different translation units - this demonstrates
// the static initialization order fiasco. The order of construction
// between global objects in different TUs is unspecified!
Counter globalCounterA("A");
Counter globalCounterB("B");

void demonstrateBad() {
    // After construction, these globals should be usable.
    // But if one tries to use the other before it's constructed,
    // we get undefined behavior.
    
    std::cout << "\n--- Bad Demo (potential static init order issue) ---\n";
    
    // This might print garbage values depending on init order!
    // If B is constructed after A, and A tries to access B during
    // its construction (not shown here but possible), it would be UB.
    
    globalCounterA.increment();
    globalCounterB.checkWithOther(globalCounterA);
}
