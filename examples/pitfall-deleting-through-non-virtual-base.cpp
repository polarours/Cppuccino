// examples/pitfall-deleting-through-non-virtual-base.cpp
// Demonstrates why deleting an object through a base pointer needs a virtual destructor.
// Compile with: g++ -std=c++17 -o pitfall_deleting_through_non_virtual_base pitfall-deleting-through-non-virtual-base.cpp

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

namespace counters {
int bases = 0;
int deriveds = 0;
}  // namespace counters

// --- The broken hierarchy: the base destructor is not virtual ---

struct BadBase {
    BadBase() { ++counters::bases; }
    ~BadBase() { --counters::bases; }  // BAD: no dynamic dispatch

    BadBase(const BadBase&) = delete;
    BadBase& operator=(const BadBase&) = delete;
};

struct BadDerived : BadBase {
    BadDerived() { ++counters::deriveds; }
    ~BadDerived() { --counters::deriveds; }  // skipped by a base-pointer delete

    BadDerived(const BadDerived&) = delete;
    BadDerived& operator=(const BadDerived&) = delete;
};

// --- The fixed hierarchy: the base destructor is virtual ---

struct GoodBase {
    GoodBase() { ++counters::bases; }
    virtual ~GoodBase() {
        --counters::bases;
        std::cout << "      ~GoodBase ran\n";
    }

    virtual std::string name() const { return "GoodBase"; }

    GoodBase(const GoodBase&) = delete;
    GoodBase& operator=(const GoodBase&) = delete;
};

struct GoodDerived : GoodBase {
    GoodDerived() { ++counters::deriveds; }
    ~GoodDerived() override {
        --counters::deriveds;
        std::cout << "      ~GoodDerived ran\n";
    }

    std::string name() const override { return "GoodDerived"; }
};

// --- 1. The broken pattern ---

void brokenDeletion() {
    std::cout << "--- 1. Deleting through a non-virtual base pointer ---\n";

    // BAD: undefined behaviour.
    // BadBase* p = new BadDerived;
    // delete p;
    //   The delete expression uses the static type BadBase*, so only
    //   ~BadBase() is called. ~BadDerived() never runs, the derived subobject
    //   is never cleaned up, and the whole thing is a formal undefined
    //   behaviour condition rather than a plain leak.

    // The defect is visible without running anything:
    static_assert(!std::has_virtual_destructor_v<BadBase>,
                  "BadBase has no virtual destructor");

    std::cout << "    BadBase has a virtual destructor: "
              << std::has_virtual_destructor_v<BadBase> << "\n";
    std::cout << "    a base-pointer delete would skip ~BadDerived\n";
}

// --- 2. The fix: a virtual destructor ---

void fixedDeletion() {
    std::cout << "\n--- 2. The fix: a virtual destructor ---\n";

    static_assert(std::has_virtual_destructor_v<GoodBase>,
                  "GoodBase has a virtual destructor");

    {
        GoodBase* p = new GoodDerived;
        std::cout << "    dynamic type: " << p->name() << "\n";
        delete p;  // OK: ~GoodDerived runs, then ~GoodBase
    }

    std::cout << "    live bases: " << counters::bases
              << ", live derived objects: " << counters::deriveds << "\n";
}

// --- 3. An interface with no data still needs a virtual destructor ---

struct ITask {
    virtual ~ITask() = default;
    virtual void run() = 0;
};

struct Task : ITask {
    void run() override { std::cout << "    the task ran\n"; }
};

void interfaceOnlyBase() {
    std::cout << "\n--- 3. An interface with no data still needs one ---\n";

    ITask* task = new Task;
    task->run();
    delete task;  // OK: the virtual destructor handles the teardown
}

// --- 4. A smart pointer does not repair the base class ---

void smartPointerDoesNotFixTheBase() {
    std::cout << "\n--- 4. A smart pointer does not repair the base class ---\n";

    // BAD: the same undefined behaviour, now hidden behind a smart pointer.
    // std::unique_ptr<BadBase> owner(new BadDerived);
    //   The default deleter calls `delete base_pointer` on a BadBase*, so
    //   ~BadDerived is skipped just the same.

    // GOOD: with a virtual destructor the default deleter is correct.
    std::unique_ptr<GoodBase> owner = std::make_unique<GoodDerived>();
    std::cout << "    unique_ptr<GoodBase> owns a " << owner->name() << "\n";
}

// --- 5. When the base is not meant to be polymorphic ---

class SealedBase {
public:
    SealedBase() = default;
    SealedBase(const SealedBase&) = delete;
    SealedBase& operator=(const SealedBase&) = delete;

protected:
    ~SealedBase() = default;  // protected: only derived types may destroy it
};

class SealedDerived final : public SealedBase {
public:
    ~SealedDerived() = default;
};

// The same idea, but keeping the polymorphic deletion semantics.
class GuardedBase {
public:
    virtual void run() {}

    // The destructor is public and virtual, so the object can be deleted
    // through the base and the teardown is still correct.
    virtual ~GuardedBase() = default;
};

class GuardedDerived final : public GuardedBase {
public:
    void run() override { std::cout << "    the guarded task ran\n"; }
};

void sealedHierarchies() {
    std::cout << "\n--- 5. When the base is not meant to be polymorphic ---\n";

    // GOOD: the hierarchy is sealed and the destructor is protected, so
    // deleting through a base pointer does not compile at all.
    // SealedBase* p = new SealedDerived;
    // delete p;
    //   error: 'SealedBase::~SealedBase()' is protected

    auto sealed = std::make_unique<SealedDerived>();
    std::cout << "    the sealed object owns its own lifetime\n";

    // A public virtual destructor keeps the teardown correct even when the
    // object is deleted through the base.
    GuardedBase* guarded = new GuardedDerived;
    guarded->run();
    delete guarded;  // OK: the destructor is virtual, so the derived part goes too
}

int main() {
    brokenDeletion();
    fixedDeletion();
    interfaceOnlyBase();
    smartPointerDoesNotFixTheBase();
    sealedHierarchies();

    std::cout << "\n=== All base-pointer deletion scenarios completed ===\n";
    return 0;
}
