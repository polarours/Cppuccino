#include "std_print.hpp"

#include <iostream>

int main() {
    std::cout << "=== Std Print Demo ===\n\n";

    std_print::Logger logger;
    logger.setLevel(std_print::Logger::Level::Debug);

    logger.debug("Initializing system");
    logger.info("Server started on port ", 8080);
    logger.warn("High memory usage: ", 85, "%");
    logger.error("Connection failed to ", "db.example.com");

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
