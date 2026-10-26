# Testing Patterns in C++

## Why Do Testing Patterns Matter?

Unit tests exist to verify logic in **isolation**, **repeatably**, and
**fast**. C++ ships no built-in test framework — the community uses Google
Test, Boost.Test, Catch2 — but whichever you pick, three core patterns stay
the same: **Mock Objects**, **Test Fixture**, and **Parameterized Tests**.

> **Core goal**: separate "the code under test" from "its dependencies", so
> each test verifies exactly one thing.

## 1. Mock Objects

### What Is a Mock?

A mock is one kind of test double: it **records calls** without performing
real side effects. It is used to verify the interaction between two objects
(call counts, arguments, order) without a real database/network/file.

> Mock records calls → Stub returns canned data → Fake is a lightweight real
> implementation.

### Implementation

```cpp
#include <functional>
#include <string>
#include <vector>

class Database {
public:
    virtual ~Database() = default;
    virtual bool connect(const std::string& url) = 0;
    virtual bool query(const std::string& sql, std::string& result) = 0;
    virtual void disconnect() = 0;
};

class MockDatabase : public Database {
public:
    bool connect(const std::string& url) override {
        connectCalls_.push_back(url);
        return shouldSucceed_;
    }

    bool query(const std::string& sql, std::string& result) override {
        queryCalls_.push_back(sql);
        if (queryHandler_) return queryHandler_(sql, result);
        result = lastQueryResult_;
        return shouldSucceed_;
    }

    void disconnect() override { disconnectCalled_ = true; }

    // --- mock control points ---
    void setShouldSucceed(bool v) { shouldSucceed_ = v; }
    void setLastQueryResult(const std::string& r) { lastQueryResult_ = r; }
    void setQueryHandler(std::function<bool(const std::string&, std::string&)> h) {
        queryHandler_ = std::move(h);
    }

    // --- assertion points ---
    std::size_t connectCallCount() const { return connectCalls_.size(); }
    std::size_t queryCallCount() const { return queryCalls_.size(); }
    bool disconnectCalled() const { return disconnectCalled_; }

private:
    bool shouldSucceed_ = true;
    bool disconnectCalled_ = false;
    std::string lastQueryResult_;
    std::vector<std::string> connectCalls_;
    std::vector<std::string> queryCalls_;
    std::function<bool(const std::string&, std::string&)> queryHandler_;
};

// the object under test
class UserRepository {
public:
    explicit UserRepository(Database& db) : db_(db) {}
    bool initialize(const std::string& url) { return db_.connect(url); }
    bool findUser(const std::string& name, std::string& out) {
        return db_.query("SELECT * FROM users WHERE name='" + name + "'", out);
    }
    void shutdown() { db_.disconnect(); }
private:
    Database& db_;
};
```

For the full runnable example, see the
[mock-objects project](../../cpp-mastery-roadmap/38-stage38/mock-objects).

### Usage Example

```cpp
#include <cassert>

MockDatabase db;
UserRepository repo(db);

db.setShouldSucceed(true);
db.setLastQueryResult("Alice, alice@example.com");

bool ok = repo.initialize("localhost:5432");
assert(ok);
assert(db.connectCallCount() == 1);

std::string result;
repo.findUser("Alice", result);
assert(result == "Alice, alice@example.com");
assert(db.queryCallCount() == 1);
```

### Mock vs Stub vs Fake

| | Mock | Stub | Fake |
|---|---|---|---|
| Purpose | verify interaction | provide canned data | lightweight real implementation |
| Behavior | records calls | returns fixed values | has actual logic |
| Typical use | "was X called?" | "return empty when not found" | in-memory database |

---

## 2. Test Fixture

### What Is a Fixture?

A fixture extracts the setup/teardown that every test in a group needs into a
shared structure, removing duplicated `setup()` / `teardown()` code and
keeping tests isolated from each other.

> **Core idea**: `setup()` prepares the data, `teardown()` cleans the state;
> every test starts from a clean slate.

### Implementation

```cpp
#include <string>
#include <vector>
#include <algorithm>

struct TodoItem {
    int id;
    std::string text;
    bool done;
};

class TodoList {
public:
    int add(const std::string& text) {
        items_.push_back({nextId_++, text, false});
        return items_.back().id;
    }
    void complete(int id) {
        for (auto& item : items_)
            if (item.id == id) { item.done = true; return; }
    }
    void remove(int id) {
        items_.erase(
            std::remove_if(items_.begin(), items_.end(),
                [id](const TodoItem& i) { return i.id == id; }),
            items_.end());
    }
    std::vector<TodoItem> all() const { return items_; }
    std::size_t size() const { return items_.size(); }

private:
    std::vector<TodoItem> items_;
    int nextId_ = 1;
};

// fixture: setup on construction, teardown on destruction
class TodoTestFixture {
public:
    TodoTestFixture() { setup(); }
    ~TodoTestFixture() { teardown(); }

    void setup() {
        list_ = TodoList();
        id1_ = list_.add("Buy milk");
        id2_ = list_.add("Walk dog");
        id3_ = list_.add("Write code");
    }

    void teardown() {
        list_ = TodoList();  // reset state
    }

    TodoList& list() { return list_; }
    int id1() const { return id1_; }
    int id2() const { return id2_; }
    int id3() const { return id3_; }

private:
    TodoList list_;
    int id1_ = 0, id2_ = 0, id3_ = 0;
};
```

For the full runnable example, see the
[test-fixture project](../../cpp-mastery-roadmap/38-stage38/test-fixture).

### Usage Example

```cpp
#include <cassert>

// every test starts from a fresh fixture
void test_add_item() {
    TodoTestFixture f;
    int newId = f.list().add("New task");
    assert(newId != 0);
    assert(f.list().size() == 4);  // 3 from setup + the new one
}

void test_complete_item() {
    TodoTestFixture f;
    f.list().complete(f.id1());       // complete "Buy milk"
    auto items = f.list().all();      // a snapshot of the current state
    assert(items.size() == 3);        // completing does not remove the item
    assert(items[0].done);            // and it is now marked done
}
```

---

## 3. Parameterized Tests

### What Are Parameterized Tests?

Run one piece of test logic over many data sets — instead of writing a test
function per data set. This is data-driven testing (DDT), C++ style.

### Implementation

```cpp
#include <functional>
#include <string>
#include <vector>

struct TestCase {
    std::string name;
    std::function<bool()> test;
};

class TestRunner {
public:
    void addTest(const std::string& name, std::function<bool()> test) {
        tests_.push_back({name, std::move(test)});
    }

    struct Results {
        std::size_t total = 0, passed = 0, failed = 0;
        std::vector<std::string> failures;
    };

    Results run() const {
        Results r;
        r.total = tests_.size();
        for (const auto& t : tests_) {
            if (t.test()) { r.passed++; }
            else { r.failed++; r.failures.push_back(t.name); }
        }
        return r;
    }

private:
    std::vector<TestCase> tests_;
};
```

For the full runnable example, see the
[parameterized-tests project](../../cpp-mastery-roadmap/38-stage38/parameterized-tests).

### Usage Example

```cpp
#include <cassert>
#include <optional>
#include <string>

std::optional<int> parseInt(const std::string& s) {
    try { return std::stoi(s); } catch (...) { return std::nullopt; }
}

void demo() {
    TestRunner runner;
    runner.addTest("parse positive", [] { return parseInt("42").has_value(); });
    runner.addTest("parse negative", [] { return parseInt("-7").has_value(); });
    runner.addTest("parse zero",     [] { return parseInt("0").has_value(); });
    runner.addTest("parse invalid",  [] { return !parseInt("abc").has_value(); });

    auto r = runner.run();
    assert(r.total == 4);
    assert(r.passed == 4);
    assert(r.failed == 0);
}
```

---

## Best Practices

1. **Mocks verify behavior, not implementation**: a mock cares "was the right
   method called", never "which private member was used"
2. **Fakes contain no branching business logic**: a fake exists to exercise
   the path, not to validate correctness
3. **Keep fixtures light**: every test re-runs `setup()` — heavy fixtures
   slow the whole suite down
4. **Parameterized data must cover the boundaries**: empty strings, extreme
   values, negatives, special characters — the data has to be representative
5. **Test isolation is the iron rule**: state a test A mutated must not
   change test B's result

## Example

A runnable version of this topic:

- [testing-patterns.cpp](../../examples/testing-patterns.cpp)

## Summary

The three patterns divide the work: mocks verify interactions, fixtures
guarantee isolation, parameterized tests sweep equivalent paths in bulk.
Together with CMake + CTest automation they are the standard testing
infrastructure of a C++ project.
