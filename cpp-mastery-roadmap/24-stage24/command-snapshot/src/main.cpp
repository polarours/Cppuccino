#include "command_snapshot.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Command Snapshot Demo ===\n\n";

    command_snapshot::TextState state;
    command_snapshot::CommandHistory history;

    history.execute(std::make_shared<command_snapshot::AppendCommand>("Hello"), state);
    history.execute(std::make_shared<command_snapshot::AppendCommand>(", World"), state);
    std::cout << "After append: " << state.text() << "\n";

    history.undo(state);
    std::cout << "After undo: " << state.text() << "\n";

    history.redo(state);
    std::cout << "After redo: " << state.text() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
