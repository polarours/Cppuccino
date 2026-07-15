#include "active_record.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_create_and_save() {
    active_record::UserRecord::clearRegistry();
    active_record::UserRecord user("Alice", "alice@test.com");
    expect(user.isNew(), "expected new before save");

    user.save();
    expect(!user.isNew(), "expected not new after save");
    expect(user.id() == 1, "expected id 1");
}

void test_find() {
    active_record::UserRecord::clearRegistry();
    active_record::UserRecord user("Bob", "bob@test.com");
    user.save();

    auto found = active_record::UserRecord::find(user.id());
    expect(found != nullptr, "expected found");
    expect(found->name() == "Bob", "expected Bob");
}

void test_find_all() {
    active_record::UserRecord::clearRegistry();
    active_record::UserRecord u1("Alice", "a@test.com");
    active_record::UserRecord u2("Bob", "b@test.com");
    u1.save();
    u2.save();

    auto all = active_record::UserRecord::findAll();
    expect(all.size() == 2, "expected 2 users");
}

void test_dirty_tracking() {
    active_record::UserRecord::clearRegistry();
    active_record::UserRecord user("Alice", "alice@test.com");
    user.save();
    expect(!user.isDirty(), "expected clean after save");

    user.setName("Alice Updated");
    expect(user.isDirty(), "expected dirty after modify");
}

} // namespace

int main() {
    try {
        std::cout << "Active Record Tests:\n";
        test_create_and_save();
        std::cout << "  test_create_and_save: PASS\n";

        test_find();
        std::cout << "  test_find: PASS\n";

        test_find_all();
        std::cout << "  test_find_all: PASS\n";

        test_dirty_tracking();
        std::cout << "  test_dirty_tracking: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "active_record_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nactive_record_tests passed" << std::endl;
    return 0;
}
