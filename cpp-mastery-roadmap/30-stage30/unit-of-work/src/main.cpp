#include "unit_of_work.hpp"

#include <iostream>

int main() {
    std::cout << "=== Unit of Work Demo ===\n\n";

    unit_of_work::UnitOfWork uow;
    uow.registerNew({1, "Alice"});
    uow.registerNew({2, "Bob"});
    uow.registerModified({1, "Alice Updated"});
    uow.commit();

    std::cout << "Committed " << uow.committed().size() << " changes\n";
    for (const auto& e : uow.committed()) {
        std::cout << "  [" << static_cast<int>(e.changeType) << "] " << e.entity.data << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
