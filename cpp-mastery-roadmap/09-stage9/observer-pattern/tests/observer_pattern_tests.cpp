#include "observer_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_observe() {
    observer_pattern::Subject<int> subject;

    std::string received;
    auto observer = std::make_shared<observer_pattern::Logger<int>>();
    subject.attach(observer);

    subject.setValue(42);
    // Logger outputs to cout
}

void test_multiple_observers() {
    observer_pattern::Subject<int> subject;

    auto counter1 = std::make_shared<observer_pattern::Counter<int>>();
    auto counter2 = std::make_shared<observer_pattern::Counter<int>>();

    subject.attach(counter1);
    subject.attach(counter2);

    subject.setValue(1);
    subject.setValue(2);

    expect(counter1->getCount() == 2, "expected counter1 count 2");
    expect(counter2->getCount() == 2, "expected counter2 count 2");
}

void test_detach() {
    observer_pattern::Subject<int> subject;

    auto counter = std::make_shared<observer_pattern::Counter<int>>();
    subject.attach(counter);

    subject.setValue(1);
    expect(counter->getCount() == 1, "expected count 1");

    subject.detach(counter);
    subject.setValue(2);
    expect(counter->getCount() == 1, "expected count 1 after detach");
}

void test_get_value() {
    observer_pattern::Subject<int> subject;

    subject.setValue(42);
    expect(subject.getValue() == 42, "expected 42");
}

void test_multiple_value_changes() {
    observer_pattern::Subject<std::string> subject;

    auto counter = std::make_shared<observer_pattern::Counter<std::string>>();
    subject.attach(counter);

    subject.setValue("first");
    subject.setValue("second");
    subject.setValue("third");

    expect(counter->getCount() == 3, "expected 3 updates");
}

void test_observer_receives_correct_value() {
    observer_pattern::Subject<int> subject;

    int lastValue = 0;
    auto observer = std::make_shared<observer_pattern::Logger<int>>();
    subject.attach(observer);

    subject.setValue(100);
    // Logger outputs to cout
}

} // namespace

int main() {
    try {
        std::cout << "Observer Pattern Tests:\n";
        test_basic_observe();
        std::cout << "  test_basic_observe: PASS\n";

        test_multiple_observers();
        std::cout << "  test_multiple_observers: PASS\n";

        test_detach();
        std::cout << "  test_detach: PASS\n";

        test_get_value();
        std::cout << "  test_get_value: PASS\n";

        test_multiple_value_changes();
        std::cout << "  test_multiple_value_changes: PASS\n";

        test_observer_receives_correct_value();
        std::cout << "  test_observer_receives_correct_value: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "observer_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nobserver_pattern_tests passed" << std::endl;
    return 0;
}
