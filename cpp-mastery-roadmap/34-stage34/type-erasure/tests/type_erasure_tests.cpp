#include "type_erasure.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_calls_heterogeneous_lambdas() {
    int calls = 0;
    std::vector<type_erasure::AnyAction> actions;
    actions.emplace_back([&] { calls += 1; });
    actions.emplace_back([&] { calls += 10; });
    for (auto& a : actions) a();
    expect(calls == 11, "both lambdas invoked");
}

void test_copy_semantics() {
    int a = 0, b = 0;
    type_erasure::AnyAction original([&] { a += 1; });
    type_erasure::AnyAction copy = original;  // deep clone of holder
    copy();
    expect(a == 1, "original callable shared target");
    original();
    expect(a == 2, "both invoke the same captured state");

    type_erasure::AnyAction assigned([] {});
    assigned = original;  // copy-assign clones the holder
    assigned();
    expect(a == 3, "assignment clones holder");
}

void test_bool_conversion() {
    type_erasure::AnyAction empty{type_erasure::AnyAction{[] {}}};
    // default-constructed via moved-from unique_ptr path
    type_erasure::AnyAction live([] {});
    expect(static_cast<bool>(live), "live action truthy");
    type_erasure::AnyAction moved = std::move(live);
    expect(static_cast<bool>(moved), "moved-to keeps holder");
    expect(!static_cast<bool>(live), "moved-from empty");
}

void test_printable_values() {
    type_erasure::AnyPrintable s("text");
    type_erasure::AnyPrintable i(123);
    type_erasure::AnyPrintable b(true);
    expect(s.str() == "text", "string passthrough");
    expect(i.str() == "123", "int via to_string");
    expect(b.str() == "true", "bool special-cased");
    expect(!s.typeName().empty(), "type name present");
    expect(s.typeName() != i.typeName(), "different types differ");
}

void test_vector_of_actions_grows() {
    std::vector<type_erasure::AnyAction> v;
    int hits = 0;
    for (int i = 0; i < 10; ++i)
        v.emplace_back([&hits] { hits += 1; });
    for (auto& a : v) a();
    expect(hits == 10, "10 copies all work (clone on growth)");
}

}  // namespace

int main() {
    try {
        test_calls_heterogeneous_lambdas();
        test_copy_semantics();
        test_bool_conversion();
        test_printable_values();
        test_vector_of_actions_grows();
        std::cout << "All type_erasure tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
