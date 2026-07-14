#include "command_router.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_basic_routing() {
    command_router::CommandRouter router;
    router.addRoute("hello", [](const std::string&) {
        return "Hello, World!";
    });

    auto result = router.route("hello");
    expect(result == "Hello, World!", "expected Hello, World!");
}

void test_unknown_command() {
    command_router::CommandRouter router;
    router.addRoute("hello", [](const std::string&) { return "Hello"; });

    auto result = router.route("unknown");
    expect(result.find("Unknown command") != std::string::npos, "expected unknown command");
}

void test_multiple_routes() {
    command_router::CommandRouter router;
    router.addRoute("add", [](const std::string& args) { return "Add: " + args; });
    router.addRoute("remove", [](const std::string& args) { return "Remove: " + args; });

    expect(router.route("add", "5") == "Add: 5", "expected Add: 5");
    expect(router.route("remove", "3") == "Remove: 3", "expected Remove: 3");
}

void test_route_count() {
    command_router::CommandRouter router;
    router.addRoute("cmd1", [](const std::string&) { return ""; });
    router.addRoute("cmd2", [](const std::string&) { return ""; });

    expect(router.routeCount() == 2, "expected 2 routes");
}

} // namespace

int main() {
    try {
        std::cout << "Command Router Tests:\n";
        test_basic_routing();
        std::cout << "  test_basic_routing: PASS\n";

        test_unknown_command();
        std::cout << "  test_unknown_command: PASS\n";

        test_multiple_routes();
        std::cout << "  test_multiple_routes: PASS\n";

        test_route_count();
        std::cout << "  test_route_count: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "command_router_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncommand_router_tests passed" << std::endl;
    return 0;
}
