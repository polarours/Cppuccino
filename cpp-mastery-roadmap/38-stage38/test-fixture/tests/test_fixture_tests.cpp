#include "test_fixture.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_fixture_setup() {
    test_fixture::TodoTestFixture fixture;
    expect(fixture.list().size() == 3, "expected 3 items after setup");
}

void test_fixture_complete() {
    test_fixture::TodoTestFixture fixture;
    fixture.list().complete(fixture.id1());

    expect(fixture.list().completedCount() == 1, "expected 1 completed");
    expect(fixture.list().all()[0].done, "expected first item done via index");
}

void test_fixture_remove() {
    test_fixture::TodoTestFixture fixture;
    fixture.list().remove(fixture.id2());

    expect(fixture.list().size() == 2, "expected 2 items after remove");
}

void test_fixture_teardown() {
    test_fixture::TodoTestFixture fixture;
    fixture.list().add("Extra item");
    fixture.teardown();

    expect(fixture.list().size() == 0, "expected 0 after teardown");
}

void test_fixture_reusable() {
    test_fixture::TodoTestFixture fixture;
    expect(fixture.list().size() == 3, "expected 3");

    fixture.teardown();
    fixture.setup();
    expect(fixture.list().size() == 3, "expected 3 after re-setup");
}

} // namespace

int main() {
    try {
        std::cout << "Test Fixture Tests:\n";
        test_fixture_setup();
        std::cout << "  test_fixture_setup: PASS\n";

        test_fixture_complete();
        std::cout << "  test_fixture_complete: PASS\n";

        test_fixture_remove();
        std::cout << "  test_fixture_remove: PASS\n";

        test_fixture_teardown();
        std::cout << "  test_fixture_teardown: PASS\n";

        test_fixture_reusable();
        std::cout << "  test_fixture_reusable: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "test_fixture_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ntest_fixture_tests passed" << std::endl;
    return 0;
}
