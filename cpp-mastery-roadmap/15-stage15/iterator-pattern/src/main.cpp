#include "iterator_pattern.hpp"

#include <iostream>

int main() {
    std::cout << "=== Iterator Pattern Demo ===\n\n";

    iterator_pattern::ConcreteAggregate<int> numbers;
    numbers.add(10);
    numbers.add(20);
    numbers.add(30);
    numbers.add(40);
    numbers.add(50);

    auto iter = numbers.createIterator();
    std::cout << "Numbers: ";
    while (iter->hasNext()) {
        std::cout << iter->next() << " ";
    }
    std::cout << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
