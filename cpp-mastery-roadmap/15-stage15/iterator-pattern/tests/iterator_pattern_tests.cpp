#include "iterator_pattern.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_iterator_basic() {
    iterator_pattern::ConcreteAggregate<int> agg;
    agg.add(1);
    agg.add(2);
    agg.add(3);

    auto iter = agg.createIterator();
    expect(iter->hasNext(), "expected hasNext");
    expect(iter->next() == 1, "expected 1");
    expect(iter->next() == 2, "expected 2");
    expect(iter->next() == 3, "expected 3");
    expect(!iter->hasNext(), "expected no more elements");
}

void test_iterator_with_strings() {
    iterator_pattern::ConcreteAggregate<std::string> agg;
    agg.add("hello");
    agg.add("world");

    auto iter = agg.createIterator();
    expect(iter->next() == "hello", "expected hello");
    expect(iter->next() == "world", "expected world");
}

void test_aggregate_size() {
    iterator_pattern::ConcreteAggregate<int> agg;
    expect(agg.size() == 0, "expected size 0");

    agg.add(1);
    agg.add(2);
    expect(agg.size() == 2, "expected size 2");
}

// Regression: the iterator used to hold a raw pointer to the aggregate, so
// using it after the aggregate was gone read freed memory.
void test_iterator_outlives_the_aggregate() {
    std::shared_ptr<iterator_pattern::Iterator<int>> it;
    {
        iterator_pattern::ConcreteAggregate<int> agg;
        agg.add(1);
        agg.add(2);
        it = agg.createIterator();
    }

    expect(it->hasNext(), "expected the first item");
    expect(it->next() == 1, "expected 1");
    expect(it->next() == 2, "expected 2");
    expect(!it->hasNext(), "expected the iterator to be done");
}

} // namespace

int main() {
    try {
        std::cout << "Iterator Pattern Tests:\n";
        test_iterator_basic();
        std::cout << "  test_iterator_basic: PASS\n";

        test_iterator_with_strings();
        std::cout << "  test_iterator_with_strings: PASS\n";

        test_aggregate_size();
        std::cout << "  test_aggregate_size: PASS\n";

        test_iterator_outlives_the_aggregate();
        std::cout << "  test_iterator_outlives_the_aggregate: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "iterator_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\niterator_pattern_tests passed" << std::endl;
    return 0;
}
