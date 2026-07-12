#include "double_dispatch.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_rock_accept() {
    double_dispatch::Rock rock;
    double_dispatch::GameVisitor visitor;
    rock.accept(visitor);

    expect(visitor.getResult() == "Rock crushes Scissors", "expected rock wins");
}

void test_paper_accept() {
    double_dispatch::Paper paper;
    double_dispatch::GameVisitor visitor;
    paper.accept(visitor);

    expect(visitor.getResult() == "Paper covers Rock", "expected paper wins");
}

void test_scissors_accept() {
    double_dispatch::Scissors scissors;
    double_dispatch::GameVisitor visitor;
    scissors.accept(visitor);

    expect(visitor.getResult() == "Scissors cuts Paper", "expected scissors wins");
}

void test_play_game() {
    double_dispatch::Rock rock;
    double_dispatch::Paper paper;

    std::string result = double_dispatch::playGame(rock, paper);
    expect(result.find("Rock crushes Scissors") != std::string::npos, "expected rock result");
    expect(result.find("Paper covers Rock") != std::string::npos, "expected paper result");
}

} // namespace

int main() {
    try {
        std::cout << "Double Dispatch Tests:\n";
        test_rock_accept();
        std::cout << "  test_rock_accept: PASS\n";

        test_paper_accept();
        std::cout << "  test_paper_accept: PASS\n";

        test_scissors_accept();
        std::cout << "  test_scissors_accept: PASS\n";

        test_play_game();
        std::cout << "  test_play_game: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "double_dispatch_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ndouble_dispatch_tests passed" << std::endl;
    return 0;
}
