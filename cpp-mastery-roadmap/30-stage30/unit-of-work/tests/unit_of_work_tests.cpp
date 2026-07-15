#include "unit_of_work.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_register_new() {
    unit_of_work::UnitOfWork uow;
    uow.registerNew({1, "Alice"});

    expect(uow.hasChanges(), "expected changes");
    expect(uow.pendingCount() == 1, "expected 1 pending");
}

void test_commit() {
    unit_of_work::UnitOfWork uow;
    uow.registerNew({1, "Alice"});
    uow.registerNew({2, "Bob"});
    uow.commit();

    expect(!uow.hasChanges(), "expected no changes after commit");
    expect(uow.committed().size() == 2, "expected 2 committed");
    expect(uow.committed()[0].changeType == unit_of_work::ChangeType::Add, "expected Add");
}

void test_modify() {
    unit_of_work::UnitOfWork uow;
    uow.registerNew({1, "Alice"});
    uow.registerModified({1, "Alice Updated"});
    uow.commit();

    expect(uow.committed().size() == 1, "expected 1 committed");
    expect(uow.committed()[0].entity.data == "Alice Updated", "expected updated data");
}

void test_remove_new_cancels() {
    unit_of_work::UnitOfWork uow;
    uow.registerNew({1, "Alice"});
    uow.registerRemoved(1);
    uow.commit();

    expect(uow.committed().empty(), "expected empty after removing new entity");
}

void test_rollback() {
    unit_of_work::UnitOfWork uow;
    uow.registerNew({1, "Alice"});
    uow.rollback();

    expect(!uow.hasChanges(), "expected no changes after rollback");
}

} // namespace

int main() {
    try {
        std::cout << "Unit of Work Tests:\n";
        test_register_new();
        std::cout << "  test_register_new: PASS\n";

        test_commit();
        std::cout << "  test_commit: PASS\n";

        test_modify();
        std::cout << "  test_modify: PASS\n";

        test_remove_new_cancels();
        std::cout << "  test_remove_new_cancels: PASS\n";

        test_rollback();
        std::cout << "  test_rollback: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "unit_of_work_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nunit_of_work_tests passed" << std::endl;
    return 0;
}
