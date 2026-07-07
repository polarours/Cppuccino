#include "simple_db.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_create_drop_table() {
    simple_db::Database db;
    expect(db.createTable("users"), "expected create to succeed");
    expect(db.tableExists("users"), "expected table to exist");
    expect(!db.createTable("users"), "expected duplicate create to fail");
    expect(db.dropTable("users"), "expected drop to succeed");
    expect(!db.tableExists("users"), "expected table to not exist");
}

void test_insert_select() {
    simple_db::Database db;
    db.createTable("users");

    simple_db::Row row1 = {{"name", "Alice"}, {"age", 25}};
    simple_db::Row row2 = {{"name", "Bob"}, {"age", 30}};

    expect(db.insert("users", row1), "expected insert to succeed");
    expect(db.insert("users", row2), "expected insert to succeed");
    expect(db.count("users") == 2, "expected count 2");

    auto rows = db.select("users");
    expect(rows.size() == 2, "expected 2 rows");
}

void test_select_with_predicate() {
    simple_db::Database db;
    db.createTable("users");

    db.insert("users", {{"name", "Alice"}, {"age", 25}});
    db.insert("users", {{"name", "Bob"}, {"age", 30}});
    db.insert("users", {{"name", "Charlie"}, {"age", 35}});

    auto adults = db.select("users", "", [](const simple_db::Row& row) {
        return std::get<int>(row.at("age")) >= 30;
    });

    expect(adults.size() == 2, "expected 2 adults");
}

void test_update() {
    simple_db::Database db;
    db.createTable("users");
    db.insert("users", {{"name", "Alice"}, {"age", 25}});

    bool updated = db.update("users", {{"age", 26}},
        [](const simple_db::Row& row) {
            return std::get<std::string>(row.at("name")) == "Alice";
        });

    expect(updated, "expected update to succeed");

    auto rows = db.select("users");
    expect(std::get<int>(rows[0].at("age")) == 26, "expected age 26");
}

void test_remove() {
    simple_db::Database db;
    db.createTable("users");
    db.insert("users", {{"name", "Alice"}, {"age", 25}});
    db.insert("users", {{"name", "Bob"}, {"age", 30}});

    bool removed = db.remove("users", [](const simple_db::Row& row) {
        return std::get<std::string>(row.at("name")) == "Alice";
    });

    expect(removed, "expected remove to succeed");
    expect(db.count("users") == 1, "expected count 1");
}

void test_save_load() {
    simple_db::Database db;
    db.createTable("users");
    db.insert("users", {{"name", "Alice"}, {"age", 25}});
    db.insert("users", {{"name", "Bob"}, {"age", 30}});

    const std::string filename = "/tmp/simple_db_test.txt";
    std::remove(filename.c_str());

    expect(db.save(filename), "expected save to succeed");

    simple_db::Database db2;
    expect(db2.load(filename), "expected load to succeed");
    expect(db2.count("users") == 2, "expected 2 users after load");

    std::remove(filename.c_str());
}

void test_count_clear() {
    simple_db::Database db;
    db.createTable("users");
    db.insert("users", {{"name", "Alice"}});
    db.insert("users", {{"name", "Bob"}});

    expect(db.count("users") == 2, "expected count 2");

    db.clear("users");
    expect(db.count("users") == 0, "expected count 0 after clear");
}

void test_insert_to_nonexistent_table() {
    simple_db::Database db;
    expect(!db.insert("nonexistent", {{"key", "value"}}), "expected insert to fail");
}

void test_multiple_tables() {
    simple_db::Database db;
    db.createTable("users");
    db.createTable("posts");

    db.insert("users", {{"name", "Alice"}});
    db.insert("posts", {{"title", "Hello"}});

    expect(db.count("users") == 1, "expected 1 user");
    expect(db.count("posts") == 1, "expected 1 post");
}

} // namespace

int main() {
    try {
        std::cout << "Simple DB Tests:\n";
        test_create_drop_table();
        std::cout << "  test_create_drop_table: PASS\n";

        test_insert_select();
        std::cout << "  test_insert_select: PASS\n";

        test_select_with_predicate();
        std::cout << "  test_select_with_predicate: PASS\n";

        test_update();
        std::cout << "  test_update: PASS\n";

        test_remove();
        std::cout << "  test_remove: PASS\n";

        test_save_load();
        std::cout << "  test_save_load: PASS\n";

        test_count_clear();
        std::cout << "  test_count_clear: PASS\n";

        test_insert_to_nonexistent_table();
        std::cout << "  test_insert_to_nonexistent_table: PASS\n";

        test_multiple_tables();
        std::cout << "  test_multiple_tables: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "simple_db_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nsimple_db_tests passed" << std::endl;
    return 0;
}
