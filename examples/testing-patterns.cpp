// examples/testing-patterns.cpp
// Demonstrates mock objects, test fixtures and parameterized tests with a small test harness.
// Compile with: g++ -std=c++17 -o testing_patterns testing-patterns.cpp

#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

// --- A very small test harness ------------------------------------------------

int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// The fixture: everything a group of tests share.
struct Fixture {
    virtual ~Fixture() = default;
    virtual void setUp() {}
    virtual void tearDown() {}
};

// --- 1. The mock ------------------------------------------------------------

// The seam: the class under test does not know where its data comes from.
class Database {
public:
    virtual ~Database() = default;
    virtual bool connect(const std::string& url) = 0;
    virtual bool query(const std::string& sql, std::string& result) = 0;
    virtual void disconnect() = 0;
};

// The mock records the calls instead of doing them.
class MockDatabase : public Database {
public:
    bool connect(const std::string& url) override {
        connectCalls.push_back(url);
        return shouldSucceed;
    }

    bool query(const std::string& sql, std::string& result) override {
        queryCalls.push_back(sql);
        if (answer.has_value()) {
            result = answer.value();
        }
        return shouldSucceed;
    }

    void disconnect() override { disconnectCalls++; }

    // The recorded calls are the thing the test looks at.
    std::vector<std::string> connectCalls;
    std::vector<std::string> queryCalls;
    int disconnectCalls = 0;
    bool shouldSucceed = true;
    std::optional<std::string> answer;
};

// The class under test.
class UserRepository {
public:
    explicit UserRepository(Database& database) : database_(database) {}

    bool load(const std::string& url, const std::string& sql,
              std::string& result) {
        if (!database_.connect(url)) {
            return false;
        }
        const bool ok = database_.query(sql, result);
        database_.disconnect();
        return ok;
    }

private:
    Database& database_;
};

void mockRecordsTheCalls() {
    std::cout << "--- 1. A mock records the calls ---\n";

    MockDatabase database;
    UserRepository repository(database);

    std::string result;
    const bool ok = repository.load("db://users", "SELECT * FROM users", result);

    expect(ok, "expected the load to succeed");
    expect(database.connectCalls.size() == 1, "expected one connect");
    expect(database.connectCalls[0] == "db://users", "expected the url");
    expect(database.queryCalls.size() == 1, "expected one query");
    expect(database.queryCalls[0] == "SELECT * FROM users", "expected the sql");
    expect(database.disconnectCalls == 1, "expected one disconnect");

    std::cout << "  the repository connected, queried and disconnected\n";
    std::cout << "  the mock remembers: " << database.connectCalls[0] << ", "
              << database.queryCalls[0] << "\n";
}

// --- 2. The failure the mock can produce -------------------------------------

void mockCanFail() {
    std::cout << "\n--- 2. The mock can also fail ---\n";

    MockDatabase database;
    database.shouldSucceed = false;

    UserRepository repository(database);
    std::string result;
    const bool ok = repository.load("db://users", "SELECT * FROM users", result);

    expect(!ok, "expected the load to fail");
    expect(database.queryCalls.empty(), "expected no query after a failed connect");

    std::cout << "  with the connect failing, no query is sent\n";
}

// --- 3. The fixture ---------------------------------------------------------

// Everything the repository tests need, set up once per test.
class RepositoryFixture : public Fixture {
public:
    void setUp() override {
        database.answer = "alice";
        ++setUpCalls;
    }

    void tearDown() override {
        ++tearDownCalls;
    }

    MockDatabase database;
    int setUpCalls = 0;
    int tearDownCalls = 0;
};

void fixtureRunsForEveryTest() {
    std::cout << "\n--- 3. A fixture runs for every test ---\n";

    RepositoryFixture fixture;
    fixture.setUp();
    expect(fixture.setUpCalls == 1, "expected one setUp");

    UserRepository repository(fixture.database);
    std::string result;
    repository.load("db://users", "SELECT name FROM users", result);
    expect(result == "alice", "expected the canned answer");

    fixture.tearDown();
    expect(fixture.tearDownCalls == 1, "expected one tearDown");

    std::cout << "  setUp ran " << fixture.setUpCalls << " time and tearDown "
              << fixture.tearDownCalls << " time\n";
    std::cout << "  the mock answered with \"" << result << "\"\n";
}

// --- 4. The parameterized test -----------------------------------------------

struct Row {
    std::string input;
    int expected;
};

std::vector<Row> rows() {
    return {{"0", 0}, {"1", 1}, {"2", 4}, {"3", 9}, {"10", 100}, {"-4", 16}};
}

int square(const std::string& text) {
    int value = std::stoi(text);
    return value * value;
}

void parameterizedTest() {
    std::cout << "\n--- 4. A parameterized test ---\n";

    int checked = 0;
    for (const Row& row : rows()) {
        const int result = square(row.input);
        if (result != row.expected) {
            ++failures;
            std::cout << "  FAIL  square(\"" << row.input << "\") was " << result
                      << ", expected " << row.expected << "\n";
            continue;
        }
        ++checked;
        std::cout << "  PASS  square(\"" << row.input << "\") == " << result
                  << "\n";
    }

    expect(checked == static_cast<int>(rows().size()),
           "expected every row to be checked");
    std::cout << "  one test body, " << rows().size()
              << " rows, and the failure names the row\n";
}

// --- 5. The tests that go wrong ----------------------------------------------

void aFailingTest() {
    std::cout << "\n--- 5. What a failing test looks like ---\n";

    MockDatabase database;
    UserRepository repository(database);

    // The mock was told to answer, but the test expects another answer.
    database.answer = "alice";
    std::string result;
    repository.load("db://users", "SELECT name FROM users", result);

    if (result != "bob") {
        std::cout << "  the test expected \"bob\" and got \"" << result
                  << "\"\n";
        std::cout << "  the harness turns that into a failure, and names the\n"
                     "  test that failed\n";
    }
}

int main() {
    std::cout << "=== Testing Patterns ===\n\n";

    mockRecordsTheCalls();
    mockCanFail();
    fixtureRunsForEveryTest();
    parameterizedTest();
    aFailingTest();

    std::cout << "\n=== All testing scenarios completed ===\n";
    std::cout << (failures == 0 ? "every check passed\n"
                                : "some checks failed\n");
    return 0;
}
