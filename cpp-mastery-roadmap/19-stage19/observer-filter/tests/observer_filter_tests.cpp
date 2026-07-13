#include "observer_filter.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_emit() {
    observer_filter::FilteredDispatcher dispatcher;

    std::string received;
    dispatcher.onEvent([&received](const observer_filter::Event& e) {
        received = e.data;
    });

    dispatcher.emit({"click", "clicked", 1});
    expect(received == "clicked", "expected clicked");
}

void test_filter_blocks() {
    observer_filter::FilteredDispatcher dispatcher;

    std::string received;
    dispatcher.onEvent([&received](const observer_filter::Event& e) {
        received = e.data;
    });

    dispatcher.addFilter([](const observer_filter::Event& e) {
        return e.priority > 5;
    });

    dispatcher.emit({"click", "low", 3});
    expect(received.empty(), "expected filtered out");

    dispatcher.emit({"click", "high", 10});
    expect(received == "high", "expected high");
}

void test_multiple_filters() {
    observer_filter::FilteredDispatcher dispatcher;

    std::string received;
    dispatcher.onEvent([&received](const observer_filter::Event& e) {
        received = e.data;
    });

    dispatcher.addFilter([](const observer_filter::Event& e) {
        return e.type == "click";
    });
    dispatcher.addFilter([](const observer_filter::Event& e) {
        return e.priority > 5;
    });

    dispatcher.emit({"hover", "data", 10});
    expect(received.empty(), "expected filtered by type");

    dispatcher.emit({"click", "data", 3});
    expect(received.empty(), "expected filtered by priority");

    dispatcher.emit({"click", "data", 10});
    expect(received == "data", "expected data");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Filter Tests:\n";
        test_basic_emit();
        std::cout << "  test_basic_emit: PASS\n";

        test_filter_blocks();
        std::cout << "  test_filter_blocks: PASS\n";

        test_multiple_filters();
        std::cout << "  test_multiple_filters: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_filter_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_filter_tests passed" << std::endl;
    return 0;
}
