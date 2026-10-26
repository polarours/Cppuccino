// examples/pitfall-dangling-string-view.cpp
// Demonstrates dangling std::string_view pitfalls and the safe owning alternatives.
// Compile with: g++ -std=c++17 -o pitfall_dangling_string_view pitfall-dangling-string-view.cpp

#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// --- 1. Returning a view to a local string ---

// BAD: the view points at 's', which is destroyed when the function returns.
// Reading the result is undefined behaviour.
std::string_view bad_view_to_local() {
    std::string s = "temporary";
    return s;
}

// GOOD: return an owning string; the caller owns the characters.
std::string good_owning_name() {
    return std::string("temporary");
}

void returningView() {
    std::cout << "--- Returning a view to a local string ---\n";

    // BAD: undefined behaviour, the local string is already gone.
    // std::string_view sv = bad_view_to_local();
    // std::cout << sv << "\n";

    // GOOD: the caller owns the characters.
    std::string name = good_owning_name();
    std::cout << "Good: " << name << "\n";
}

// --- 2. Returning a view to a temporary result ---

// BAD: the temporary is destroyed at the end of the return statement.
std::string_view bad_view_to_temporary() {
    return std::string("x") + "y";
}

void returningTemporaryView() {
    std::cout << "\n--- Returning a view to a temporary ---\n";

    // BAD: undefined behaviour, the temporary died inside the callee.
    // std::string_view sv = bad_view_to_temporary();
    // std::cout << sv << "\n";

    // GOOD: build the result as an owning value.
    std::string joined = std::string("x") + "y";
    std::cout << "Good: " << joined << "\n";
}

// --- 3. Storing a view in a longer-lived object ---

struct BadHolder {
    std::string_view text;  // BAD: the holder does not own the characters
};

struct GoodHolder {
    std::string text;  // GOOD: the holder owns the characters
};

void storingView() {
    std::cout << "\n--- Storing a view in a longer-lived object ---\n";

    // BAD: the temporary dies at the end of the full expression, so
    // holder.text would dangle.
    // BadHolder holder{std::string("payload")};
    // std::cout << holder.text << "\n";

    // GOOD: the holder owns its text.
    GoodHolder holder{std::string("payload")};
    std::cout << "Good: " << holder.text << "\n";
}

// --- 4. Mutating the owner invalidates the view ---

void mutatingOwner() {
    std::cout << "\n--- Mutating the owner invalidates the view ---\n";

    std::string owner = "a string long enough to live on the heap, not in the SSO buffer";
    std::string_view view = owner;  // borrows the owner's buffer

    std::cout << "Before the mutation the view has " << view.size() << " characters\n";

    owner.reserve(4096);  // forces a reallocation: the old buffer is released

    // BAD: reading 'view' now touches released storage: undefined behaviour.
    // std::cout << view << "\n";

    // The view object is still alive, but its pointer and length are stale.
    std::cout << "After the mutation the view still reports " << view.size()
              << " characters, but its bytes belong to the released buffer\n";

    // GOOD: reacquire the view after mutating the owner.
    std::string_view fresh = owner;
    std::cout << "Good: reacquired view: " << fresh << "\n";
}

// --- 5. substr produces another view ---

void substrChaining() {
    std::cout << "\n--- substr produces another view ---\n";

    std::string owner = "abcdef";
    std::string_view whole = owner;
    std::string_view part = whole.substr(1, 3);  // still borrows 'owner'

    // GOOD: 'owner' is alive, so the derived view is valid.
    std::cout << "Good: substr view = " << part << "\n";

    // BAD: deriving a view from a temporary: the temporary dies immediately.
    // std::string_view bad = std::string("abcdef").substr(1, 3);
    // std::cout << bad << "\n";
}

// --- 6. Converting at an ownership boundary ---

// A queue that outlives the call that filled it.
std::vector<std::string> g_queue;

void enqueue(std::string message) {
    g_queue.push_back(std::move(message));
}

void enqueue_view(std::string_view sv) {
    // GOOD: the ownership boundary is explicit, the queue owns a copy.
    enqueue(std::string(sv));
}

void ownershipBoundary() {
    std::cout << "\n--- Converting at an ownership boundary ---\n";

    std::string local = "message built in a short-lived scope";
    enqueue_view(local);  // safe: the queue stores its own copy

    std::cout << "Good: queue holds " << g_queue.size() << " owned message(s): "
              << g_queue.front() << "\n";
}

// --- 7. string_view as a read-only parameter ---

void log_line(std::string_view msg) {
    std::cout << "[log] " << msg << "\n";
}

void goodParameterUse() {
    std::cout << "\n--- string_view as a read-only parameter ---\n";

    std::string owned = "a std::string argument";
    log_line(owned);        // no copy, the callee does not store the view
    log_line("a string literal argument");  // no temporary is created either
}

int main() {
    returningView();
    returningTemporaryView();
    storingView();
    mutatingOwner();
    substrChaining();
    ownershipBoundary();
    goodParameterUse();

    std::cout << "\n=== All dangling string_view scenarios completed ===\n";
    return 0;
}
