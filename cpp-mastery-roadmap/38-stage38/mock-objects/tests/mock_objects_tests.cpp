#include "mock_objects.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_mock_connect() {
    mock_objects::MockDatabase mock;
    mock_objects::UserRepository repo(mock);

    expect(repo.initialize("localhost"), "expected connect success");
    expect(mock.connectCallCount() == 1, "expected 1 connect call");
    expect(mock.connectCalls()[0] == "localhost", "expected localhost");
}

void test_mock_query() {
    mock_objects::MockDatabase mock;
    mock.setLastQueryResult("Alice:30");
    mock_objects::UserRepository repo(mock);

    std::string result;
    expect(repo.findUser("Alice", result), "expected query success");
    expect(result == "Alice:30", "expected Alice:30");
    expect(mock.queryCallCount() == 1, "expected 1 query call");
    expect(mock.queryCalls()[0].find("Alice") != std::string::npos, "expected Alice in query");
}

void test_mock_failure() {
    mock_objects::MockDatabase mock;
    mock.setShouldSucceed(false);
    mock_objects::UserRepository repo(mock);

    expect(!repo.initialize("bad"), "expected connect failure");
}

void test_mock_custom_handler() {
    mock_objects::MockDatabase mock;
    mock.setQueryHandler([](const std::string& sql, std::string& result) {
        if (sql.find("ERROR") != std::string::npos) return false;
        result = "found";
        return true;
    });

    std::string result;
    expect(mock.query("SELECT *", result), "expected success");
    expect(result == "found", "expected found");

    expect(!mock.query("SELECT ERROR", result), "expected failure");
}

void test_mock_disconnect() {
    mock_objects::MockDatabase mock;
    mock_objects::UserRepository repo(mock);

    repo.shutdown();
    expect(mock.disconnectCalled(), "expected disconnect called");
}

} // namespace

int main() {
    try {
        std::cout << "Mock Objects Tests:\n";
        test_mock_connect();
        std::cout << "  test_mock_connect: PASS\n";

        test_mock_query();
        std::cout << "  test_mock_query: PASS\n";

        test_mock_failure();
        std::cout << "  test_mock_failure: PASS\n";

        test_mock_custom_handler();
        std::cout << "  test_mock_custom_handler: PASS\n";

        test_mock_disconnect();
        std::cout << "  test_mock_disconnect: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "mock_objects_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nmock_objects_tests passed" << std::endl;
    return 0;
}
