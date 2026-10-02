#include "type_erasure.hpp"

#include <iostream>
#include <vector>

int main() {
    // 1) container of heterogeneous callables
    std::vector<type_erasure::AnyAction> actions;
    actions.emplace_back([] { std::cout << "action: greet\n"; });
    actions.emplace_back([n = 3] {
        for (int i = 0; i < n; ++i) std::cout << "action: tick " << i << "\n";
    });

    std::cout << "actions: " << actions.size() << "\n";
    for (auto& a : actions) a();

    // 2) type-erased printable values
    std::vector<type_erasure::AnyPrintable> row{
        "cppuccino", 42, true, 3.14};
    for (auto& v : row)
        std::cout << "value=" << v.str() << "\n";

    return 0;
}
