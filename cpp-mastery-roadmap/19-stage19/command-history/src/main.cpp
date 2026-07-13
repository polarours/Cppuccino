#include "command_history.hpp"

#include <iostream>

int main() {
    std::cout << "=== Command History Demo ===\n\n";

    command_history::HistoryManager history;

    history.add("ls -la", "file1.txt file2.txt");
    history.add("pwd", "/home/user");
    history.add("echo hello", "hello");

    std::cout << "History (" << history.size() << " entries):\n";
    for (const auto& entry : history.getAll()) {
        std::cout << "  " << entry.command << " -> " << entry.result << "\n";
    }

    history.undo();
    std::cout << "\nAfter undo (" << history.size() << " entries):\n";
    auto last = history.getLast();
    std::cout << "  Last: " << last.command << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
