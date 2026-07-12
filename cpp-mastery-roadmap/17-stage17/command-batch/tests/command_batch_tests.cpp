#include "command_batch.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_single_command() {
    int counter = 0;
    auto cmd = std::make_shared<command_batch::IncrementCommand>(counter);

    cmd->execute();
    expect(counter == 1, "expected 1");

    cmd->undo();
    expect(counter == 0, "expected 0");
}

void test_batch_execute() {
    int counter = 0;
    auto batch = std::make_shared<command_batch::BatchCommand>();

    batch->add(std::make_shared<command_batch::IncrementCommand>(counter));
    batch->add(std::make_shared<command_batch::IncrementCommand>(counter));
    batch->add(std::make_shared<command_batch::IncrementCommand>(counter));

    batch->execute();
    expect(counter == 3, "expected 3");
    expect(batch->size() == 3, "expected 3 commands");
}

void test_batch_undo() {
    int counter = 0;
    auto batch = std::make_shared<command_batch::BatchCommand>();

    batch->add(std::make_shared<command_batch::IncrementCommand>(counter));
    batch->add(std::make_shared<command_batch::IncrementCommand>(counter));
    batch->add(std::make_shared<command_batch::DecrementCommand>(counter));

    batch->execute();
    expect(counter == 1, "expected 1");

    batch->undo();
    expect(counter == 0, "expected 0");
}

} // namespace

int main() {
    try {
        std::cout << "Command Batch Tests:\n";
        test_single_command();
        std::cout << "  test_single_command: PASS\n";

        test_batch_execute();
        std::cout << "  test_batch_execute: PASS\n";

        test_batch_undo();
        std::cout << "  test_batch_undo: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_batch_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_batch_tests passed" << std::endl;
    return 0;
}
