#include "command_batch.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Command Batch Demo ===\n\n";

    int counter = 0;
    auto batch = std::make_shared<command_batch::BatchCommand>();

    batch->add(std::make_shared<command_batch::IncrementCommand>(counter));
    batch->add(std::make_shared<command_batch::IncrementCommand>(counter));
    batch->add(std::make_shared<command_batch::IncrementCommand>(counter));
    batch->add(std::make_shared<command_batch::DecrementCommand>(counter));

    std::cout << "Initial counter: " << counter << "\n";

    batch->execute();
    std::cout << "After execute: " << counter << "\n";

    batch->undo();
    std::cout << "After undo: " << counter << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
