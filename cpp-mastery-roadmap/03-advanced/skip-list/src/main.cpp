#include "skip_list.hpp"

#include <iostream>

int main() {
    skip_list::SkipList<int> sl;
    for (int v : {30, 10, 50, 20, 40, 10}) sl.insert(v);  // 10 duplicate rejected

    std::cout << "size: " << sl.size() << "\nordered:";
    for (int v : sl.toVector()) std::cout << " " << v;
    std::cout << "\n";

    auto after20 = sl.successor(20);
    std::cout << "successor of 20: "
              << (after20 ? std::to_string(*after20) : "none") << "\n";
    std::cout << "contains 30: " << std::boolalpha << sl.contains(30) << "\n";
    return 0;
}
