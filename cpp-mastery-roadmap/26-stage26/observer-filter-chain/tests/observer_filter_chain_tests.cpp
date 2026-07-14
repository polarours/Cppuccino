#include "observer_filter_chain.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_no_filters() {
    observer_filter_chain::FilterChain chain;
    observer_filter_chain::FilteredDispatcher dispatcher(chain);

    std::string received;
    dispatcher.setHandler([&received](const observer_filter_chain::Event& e) {
        received = e.data;
    });

    dispatcher.emit({"msg", "hello"});
    expect(received == "hello", "expected hello");
}

void test_single_filter() {
    observer_filter_chain::FilterChain chain;
    chain.addFilter([](const observer_filter_chain::Event& e) {
        return observer_filter_chain::Event{e.type, "filtered:" + e.data};
    });

    observer_filter_chain::FilteredDispatcher dispatcher(chain);

    std::string received;
    dispatcher.setHandler([&received](const observer_filter_chain::Event& e) {
        received = e.data;
    });

    dispatcher.emit({"msg", "hello"});
    expect(received == "filtered:hello", "expected filtered:hello");
}

void test_chained_filters() {
    observer_filter_chain::FilterChain chain;
    chain.addFilter([](const observer_filter_chain::Event& e) {
        return observer_filter_chain::Event{e.type, e.data + "!"};
    });
    chain.addFilter([](const observer_filter_chain::Event& e) {
        return observer_filter_chain::Event{e.type, e.data + "!"};
    });

    observer_filter_chain::FilteredDispatcher dispatcher(chain);

    std::string received;
    dispatcher.setHandler([&received](const observer_filter_chain::Event& e) {
        received = e.data;
    });

    dispatcher.emit({"msg", "hello"});
    expect(received == "hello!!", "expected hello!!");
}

} // namespace

int main() {
    try {
        std::cout << "Observer Filter Chain Tests:\n";
        test_no_filters();
        std::cout << "  test_no_filters: PASS\n";

        test_single_filter();
        std::cout << "  test_single_filter: PASS\n";

        test_chained_filters();
        std::cout << "  test_chained_filters: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "observer_filter_chain_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nobserver_filter_chain_tests passed" << std::endl;
    return 0;
}
