#include "chain_of_responsibility.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Chain of Responsibility Demo ===\n\n";

    // Build chain
    auto auth = std::make_shared<chain_of_responsibility::AuthHandler>();
    auto logging = std::make_shared<chain_of_responsibility::LoggingHandler>();
    auto validation = std::make_shared<chain_of_responsibility::ValidationHandler>();
    auto processing = std::make_shared<chain_of_responsibility::ProcessingHandler>();

    auth->setNext(logging);
    logging->setNext(validation);
    validation->setNext(processing);

    // Process request
    chain_of_responsibility::Request req{"Hello World", 1};
    auth->handle(req);

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
