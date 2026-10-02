#include "union_find.hpp"

#include <iostream>

int main() {
    union_find::DSU dsu(7);  // 0..6
    dsu.unite(0, 1);
    dsu.unite(1, 2);
    dsu.unite(3, 4);

    std::cout << std::boolalpha;
    std::cout << "0 <-> 2: " << dsu.connected(0, 2) << "\n";
    std::cout << "0 <-> 3: " << dsu.connected(0, 3) << "\n";
    std::cout << "components: " << dsu.componentCount() << " (of "
              << dsu.size() << ")\n";
    dsu.unite(2, 3);
    std::cout << "after merge 2-3, components: " << dsu.componentCount() << "\n";
    return 0;
}
