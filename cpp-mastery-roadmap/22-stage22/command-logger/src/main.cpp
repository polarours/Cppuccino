#include "command_logger.hpp"

#include <iostream>

int main() {
    std::cout << "=== Command Logger Demo ===\n\n";

    command_logger::CommandLogger logger;
    logger.log("ls", "file1.txt", true);
    logger.log("rm", "deleted", false);
    logger.log("cp", "copied", true);

    logger.printAll();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
