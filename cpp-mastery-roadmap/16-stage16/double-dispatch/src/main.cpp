#include "double_dispatch.hpp"

#include <iostream>

int main() {
    std::cout << "=== Double Dispatch Demo ===\n\n";

    double_dispatch::Rock rock;
    double_dispatch::Paper paper;
    double_dispatch::Scissors scissors;

    std::cout << "Rock vs Paper: " << double_dispatch::playGame(rock, paper) << "\n";
    std::cout << "Paper vs Scissors: " << double_dispatch::playGame(paper, scissors) << "\n";
    std::cout << "Scissors vs Rock: " << double_dispatch::playGame(scissors, rock) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
