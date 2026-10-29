#include "observer_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// The classes under test report what they do by printing, so the output
// is what the test looks at.
class CapturedOutput {
public:
    CapturedOutput() : previous_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CapturedOutput() { std::cout.rdbuf(previous_); }

    CapturedOutput(const CapturedOutput&) = delete;
    CapturedOutput& operator=(const CapturedOutput&) = delete;

    std::string text() const { return buffer_.str(); }

private:
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

// A minimal observer that keeps the value it received, so a test can check
// what the subject handed over.
class ValueRecorder : public observer_pattern::Observer<int> {
public:
    void update(const int& value) override { lastValue = value; }

    int lastValue = 0;
};

void test_basic_observe() {
    observer_pattern::Subject<int> subject;

    auto observer = std::make_shared<observer_pattern::Logger<int>>();
    subject.attach(observer);

    CapturedOutput output;
    subject.setValue(42);

    const std::string text = output.text();
    expect(text.find("[Logger] Value changed to: 42") != std::string::npos,
           "expected the logger to report the new value, got: " + text);
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

    auto logger = std::make_shared<observer_pattern::Logger<int>>();
    auto recorder = std::make_shared<ValueRecorder>();
    subject.attach(logger);
    subject.attach(recorder);

    subject.setValue(100);
    expect(recorder->lastValue == 100, "expected 100");
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
