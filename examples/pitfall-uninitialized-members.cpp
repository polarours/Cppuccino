// examples/pitfall-uninitialized-members.cpp
// Demonstrates uninitialized member variables and the habits that prevent them.
// Compile with: g++ -std=c++17 -o pitfall_uninitialized_members pitfall-uninitialized-members.cpp

#include <iostream>
#include <string>
#include <type_traits>

// --- 1. Members without an initializer ---

struct Bad {
    int x;    // indeterminate after default-initialization
    float y;  // indeterminate
    int* ptr; // indeterminate
};

struct Good {
    int x = 0;
    float y = 0.0f;
    int* ptr = nullptr;
    std::string name;  // default-constructed: an empty string
};

void valueInitializationZeroesMembers() {
    std::cout << "--- 1. Value-initialization zeroes the members ---\n";

    // `Bad b{}` is value-initialization: an aggregate without a user-provided
    // constructor is zero-initialized, so reading the members is defined.
    Bad b{};
    std::cout << "  Bad b{} -> x = " << b.x << ", y = " << b.y
              << ", ptr = " << (b.ptr == nullptr ? "nullptr" : "not null") << "\n";

    // `Bad c;` is default-initialization: the members keep whatever the
    // memory happened to hold. Reading them is undefined behaviour, so it
    // is not done here.
    Bad c;
    (void)c;
    std::cout << "  Bad c;  -> members are indeterminate (not read here)\n";

    // The type traits say the same thing: `Bad` has a trivial default
    // constructor, so the compiler does not touch the members at all.
    static_assert(std::is_trivially_default_constructible<Bad>::value,
                  "Bad is trivially default constructible");
    static_assert(!std::is_trivially_default_constructible<Good>::value,
                  "Good has a non-trivial default constructor");
    std::cout << "  Bad is trivially default constructible, Good is not\n";
}

// --- 2. A constructor that forgets a member ---

class PartialInit {
public:
    explicit PartialInit(int a) : a_(a) {}  // BAD: b_ and ptr_ are left indeterminate
    int a() const { return a_; }

private:
    int a_;
    int b_;
    int* ptr_;
};

class FullInit {
public:
    explicit FullInit(int a) : a_(a) {}  // GOOD: b_ and ptr_ have initializers
    int a() const { return a_; }
    int b() const { return b_; }
    int* ptr() const { return ptr_; }

private:
    int a_;
    int b_ = 0;
    int* ptr_ = nullptr;
};

void forgottenMembers() {
    std::cout << "\n--- 2. A constructor that forgets a member ---\n";

    // BAD: reading b_ or ptr_ is undefined behaviour.
    // PartialInit partial(1);
    // std::cout << "  b = " << partial.b() << "\n";

    // GOOD: every member is initialized at its declaration.
    FullInit full(1);
    std::cout << "  full.a() = " << full.a() << ", full.b() = " << full.b()
              << ", full.ptr() = " << (full.ptr() == nullptr ? "nullptr" : "not null") << "\n";
}

// --- 3. `= default` is fine when the members have initializers ---

class ExplicitDefaults {
public:
    ExplicitDefaults() = default;  // OK: every member has an initializer
    int value() const { return value_; }
    int* ptr() const { return ptr_; }

private:
    int value_ = 0;
    int* ptr_ = nullptr;
};

void defaultConstructor() {
    std::cout << "\n--- 3. `= default` is fine when the members have initializers ---\n";

    ExplicitDefaults object;
    std::cout << "  object.value() = " << object.value()
              << ", object.ptr() = " << (object.ptr() == nullptr ? "nullptr" : "not null") << "\n";

    // A const member has to be initialized in the constructor -- the
    // compiler rejects any constructor that leaves it out:
    // class WithConst { const int id_; public: explicit WithConst(int id) : id_(id) {} };
}

int main() {
    valueInitializationZeroesMembers();
    forgottenMembers();
    defaultConstructor();

    std::cout << "\n--- 4. The good habit, in one line ---\n";
    Good good;
    std::cout << "  good.x = " << good.x << ", good.y = " << good.y
              << ", good.ptr = " << (good.ptr == nullptr ? "nullptr" : "not null")
              << ", good.name = '" << good.name << "'\n";

    std::cout << "\n=== All uninitialized member scenarios completed ===\n";
    return 0;
}
