#include "strategy_combinator.hpp"

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

void test_all_mode() {
    strategy_combinator::PredicateCombinator combinator(strategy_combinator::CombineMode::All);
    combinator.add(std::make_shared<strategy_combinator::EvenPredicate>());
    combinator.add(std::make_shared<strategy_combinator::PositivePredicate>());

    expect(combinator.evaluate(4), "expected 4 (even and positive)");
    expect(!combinator.evaluate(-2), "expected false (-2 not positive)");
    expect(!combinator.evaluate(3), "expected false (3 not even)");
}

void test_any_mode() {
    strategy_combinator::PredicateCombinator combinator(strategy_combinator::CombineMode::Any);
    combinator.add(std::make_shared<strategy_combinator::EvenPredicate>());
    combinator.add(std::make_shared<strategy_combinator::PositivePredicate>());

    expect(combinator.evaluate(3), "expected 3 (positive)");
    expect(combinator.evaluate(-2), "expected -2 (even)");
    expect(!combinator.evaluate(-1), "expected false (-1 neither)");
}

void test_filter() {
    strategy_combinator::PredicateCombinator combinator(strategy_combinator::CombineMode::All);
    combinator.add(std::make_shared<strategy_combinator::EvenPredicate>());
    combinator.add(std::make_shared<strategy_combinator::PositivePredicate>());

    auto result = combinator.filter({-3, -2, -1, 0, 1, 2, 3, 4});
    expect(result.size() == 2, "expected 2 filtered");
    expect(result[0] == 2, "expected 2");
    expect(result[1] == 4, "expected 4");
}

void test_greater_than() {
    strategy_combinator::PredicateCombinator combinator(strategy_combinator::CombineMode::All);
    combinator.add(std::make_shared<strategy_combinator::GreaterThanPredicate>(5));

    expect(combinator.evaluate(10), "expected true");
    expect(!combinator.evaluate(3), "expected false");
}

} // namespace

int main() {
    try {
        std::cout << "Strategy Combinator Tests:\n";
        test_all_mode();
        std::cout << "  test_all_mode: PASS\n";

        test_any_mode();
        std::cout << "  test_any_mode: PASS\n";

        test_filter();
        std::cout << "  test_filter: PASS\n";

        test_greater_than();
        std::cout << "  test_greater_than: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "strategy_combinator_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nstrategy_combinator_tests passed" << std::endl;
    return 0;
}
