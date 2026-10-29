#include "concepts_pattern.hpp"

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

void test_sum() {
    std::vector<int> ints = {1, 2, 3, 4, 5};
    expect(concepts_pattern::sum(ints) == 15, "expected 15");

    std::vector<double> doubles = {1.5, 2.5, 3.0};
    expect(concepts_pattern::sum(doubles) == 7.0, "expected 7.0");
}

void test_average() {
    std::vector<int> values = {2, 4, 6, 8};
    expect(concepts_pattern::average(values) == 5, "expected 5");

    std::vector<int> empty;
    expect(concepts_pattern::average(empty) == 0, "expected 0 for empty");
}

void test_describe() {
    concepts_pattern::Point p(3.0, 4.0);
    auto result = concepts_pattern::describe(p);
    expect(result.find("3.0") != std::string::npos, "expected 3.0 in description");
    expect(result.find("4.0") != std::string::npos, "expected 4.0 in description");
}

void test_print_container() {
    std::vector<int> v = {10, 20, 30};

    CapturedOutput output;
    concepts_pattern::printContainer(v);

    const std::string text = output.text();
    expect(text.find("10, 20, 30") != std::string::npos,
           "expected the values to be printed, got: " + text);
}

void test_concepts_compile() {
    static_assert(concepts_pattern::Numeric<int>, "int is Numeric");
    static_assert(concepts_pattern::Numeric<double>, "double is Numeric");
    static_assert(!concepts_pattern::Numeric<std::string>, "string is not Numeric");

    // A concept that accepts everything accepts nothing.
    expect(concepts_pattern::HasToString<concepts_pattern::Point>,
           "expected Point to have a toString");
    expect(!concepts_pattern::HasToString<concepts_pattern::NoToString>,
           "expected NoToString not to have a toString");
    expect(concepts_pattern::Container<std::vector<int>>,
           "expected a vector to be a container");
    expect(!concepts_pattern::Container<int>,
           "expected int not to be a container");
}

} // namespace

int main() {
    try {
        std::cout << "Concepts Pattern Tests:\n";
        test_sum();
        std::cout << "  test_sum: PASS\n";

        test_average();
        std::cout << "  test_average: PASS\n";

        test_describe();
        std::cout << "  test_describe: PASS\n";

        test_print_container();
        std::cout << "  test_print_container: PASS\n";

        test_concepts_compile();
        std::cout << "  test_concepts_compile: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "concepts_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nconcepts_pattern_tests passed" << std::endl;
    return 0;
}
