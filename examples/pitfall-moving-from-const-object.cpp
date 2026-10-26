// examples/pitfall-moving-from-const-object.cpp
// Demonstrates that std::move on a const object copies, and shows the sink-API patterns that really move.
// Compile with: g++ -std=c++17 -o pitfall_moving_from_const_object pitfall-moving-from-const-object.cpp

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

// --- An instrumented type ----------------------------------------------------

class Tracked {
public:
    Tracked() = default;
    explicit Tracked(std::string text) : payload_(std::move(text)) {}

    Tracked(const Tracked& other) : payload_(other.payload_) {
        std::cout << "      [copy constructor] the source was copied\n";
    }

    Tracked(Tracked&& other) noexcept : payload_(std::move(other.payload_)) {
        std::cout << "      [move constructor] the source was stolen from\n";
    }

    Tracked& operator=(const Tracked& other) {
        payload_ = other.payload_;
        std::cout << "      [copy assignment] the source was copied\n";
        return *this;
    }

    Tracked& operator=(Tracked&& other) noexcept {
        payload_ = std::move(other.payload_);
        std::cout << "      [move assignment] the source was stolen from\n";
        return *this;
    }

    const std::string& payload() const { return payload_; }
    bool empty() const { return payload_.empty(); }

private:
    std::string payload_;
};

// --- 1. std::move is only a cast ---------------------------------------------

void move_is_a_cast() {
    std::cout << "--- 1. std::move is only a cast ---\n";

    // std::move keeps the const qualification of its argument, so the result
    // is const Tracked&& and not Tracked&&.
    static_assert(std::is_same_v<decltype(std::move(std::declval<const Tracked&>())),
                                 const Tracked&&>);
    static_assert(std::is_same_v<decltype(std::move(std::declval<Tracked&>())),
                                 Tracked&&>);

    std::cout << "std::move(const T&) has type const T&&\n";
    std::cout << "std::move(T&)     has type T&&\n";
    std::cout << "no memory is transferred inside std::move itself\n";
}

// --- 2. Moving from a const object copies ------------------------------------

void take_const_reference(const Tracked& source) {
    // std::move keeps the const qualification, so the expression below has
    // the type const Tracked&& and cannot bind to the move constructor.
    // (GCC even reports a "redundant move" for a plain std::move here.)
    const Tracked&& as_const_rvalue = std::move(source);
    Tracked local = as_const_rvalue;  // the copy constructor is selected
}

// Generic code hits the same trap, because the constness is only known when
// the template is instantiated.
template <typename T>
void take_by_forwarding_reference(T&& source) {
    Tracked local = std::move(source);  // looks like a move, copies for const T&
    std::cout << "      the copy/move above is what the caller gets\n";
}

void moving_from_const_copies() {
    std::cout << "\n--- 2. Moving from a const object copies ---\n";

    const Tracked source("payload");
    std::cout << "  calling take_const_reference(source):\n";
    take_const_reference(source);

    std::cout << "  the source still holds its payload: "
              << (source.empty() ? "no" : "yes") << "\n";
    std::cout << "  a real move would have left the source empty\n";

    std::cout << "  calling a forwarding template with the same source:\n";
    take_by_forwarding_reference(source);
}

// --- 3. The false-optimization sink API --------------------------------------

class NameHolder {
public:
    // BAD: the parameter is const, so std::move cannot reach the move
    // assignment operator.  The code looks optimized but copies.
    void set_name_suspect(const std::string& text) {
        name_ = std::move(text);
    }

    // GOOD: pass by value and move inside.  Rvalues move into the parameter,
    // lvalues copy into it exactly once.
    void set_name_sink(std::string text) {
        name_ = std::move(text);
    }

    // The explicit overload pair, when the two costs must be visible.
    void set_name_pair(const std::string& text) {
        name_ = text;
    }

    void set_name_pair(std::string&& text) noexcept {
        name_ = std::move(text);
    }

    const std::string& name() const { return name_; }

private:
    std::string name_;
};

void sink_api_patterns() {
    std::cout << "\n--- 3. The false-optimization sink API ---\n";

    NameHolder holder;

    std::string source("Alice");
    holder.set_name_suspect(source);
    std::cout << "  after set_name_suspect, the caller's string is "
              << (source.empty() ? "empty" : "still intact") << "\n";

    std::string rvalue_source("Bob");
    holder.set_name_sink(std::move(rvalue_source));
    std::cout << "  after set_name_sink, the caller's string is "
              << (rvalue_source.empty() ? "empty (a real move)" : "still intact")
              << "\n";

    std::string lvalue_source("Carol");
    holder.set_name_sink(lvalue_source);
    std::cout << "  after set_name_sink with an lvalue, the caller's string is "
              << (lvalue_source.empty() ? "empty" : "still intact")
              << " (one copy is unavoidable)\n";
    std::cout << "  the holder now stores: " << holder.name() << "\n";
}

// --- 4. Perfect forwarding does not remove const -----------------------------

template <typename T>
void forward_to_sink(T&& value) {
    Tracked local = std::forward<T>(value);
}

void forwarding_keeps_constness() {
    std::cout << "\n--- 4. Perfect forwarding does not remove const ---\n";

    const Tracked const_source("const source");
    std::cout << "  forwarding a const lvalue:\n";
    forward_to_sink(const_source);

    Tracked mutable_source("mutable source");
    std::cout << "  forwarding a non-const lvalue:\n";
    forward_to_sink(mutable_source);

    std::cout << "  forwarding an rvalue:\n";
    forward_to_sink(Tracked("temporary"));
}

// --- 5. What to check in review ---------------------------------------------

void review_checklist() {
    std::cout << "\n--- 5. Review checklist ---\n";
    std::cout << "  * a const T& parameter plus std::move is a copy in disguise\n";
    std::cout << "  * a sink API takes its argument by value and moves internally\n";
    std::cout << "  * returning const T suppresses the move as well\n";
    std::cout << "  * only trust a measured copy/move count, never the syntax\n";
}

int main() {
    move_is_a_cast();
    moving_from_const_copies();
    sink_api_patterns();
    forwarding_keeps_constness();
    review_checklist();

    std::cout << "\n=== All move-from-const scenarios completed ===\n";
    return 0;
}
