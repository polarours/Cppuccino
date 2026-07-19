#include "deducing_this.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_fluent_builder() {
    deducing_this::Builder builder;
    auto person = builder.withName("Alice").withAge(30).withEmail("alice@test.com");

    expect(person.name() == "Alice", "expected Alice");
    expect(person.age() == 30, "expected 30");
    expect(person.email() == "alice@test.com", "expected alice@test.com");
}

void test_builder_describe() {
    deducing_this::Builder builder;
    auto person = builder.withName("Bob").withAge(25);

    expect(person.describe().find("Bob") != std::string::npos, "expected Bob in describe");
}

void test_counter_fluent() {
    deducing_this::Counter counter;
    auto result = counter.increment().add(5).increment();

    expect(result.value() == 7, "expected 7 (0 + 1 + 5 + 1)");
}

void test_counter_chaining() {
    deducing_this::Counter counter;
    auto result = counter.add(10).add(20);

    expect(result.value() == 30, "expected 30");
}

} // namespace

int main() {
    try {
        std::cout << "Deducing This Tests:\n";
        test_fluent_builder();
        std::cout << "  test_fluent_builder: PASS\n";

        test_builder_describe();
        std::cout << "  test_builder_describe: PASS\n";

        test_counter_fluent();
        std::cout << "  test_counter_fluent: PASS\n";

        test_counter_chaining();
        std::cout << "  test_counter_chaining: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "deducing_this_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ndeducing_this_tests passed" << std::endl;
    return 0;
}
