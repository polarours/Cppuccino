#include "simple_db.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== Simple Database Demo ===\n\n";

    simple_db::Database db;

    // Create tables
    db.createTable("users");
    db.createTable("posts");

    // Insert data
    std::cout << "Inserting data...\n";
    db.insert("users", {{"name", "Alice"}, {"age", 25}, {"email", "alice@example.com"}});
    db.insert("users", {{"name", "Bob"}, {"age", 30}, {"email", "bob@example.com"}});
    db.insert("users", {{"name", "Charlie"}, {"age", 35}, {"email", "charlie@example.com"}});

    db.insert("posts", {{"title", "Hello World"}, {"author", "Alice"}});
    db.insert("posts", {{"title", "C++ Tips"}, {"author", "Bob"}});

    // Query data
    std::cout << "\nAll users:\n";
    auto allUsers = db.select("users");
    for (const auto& row : allUsers) {
        std::cout << "  " << std::get<std::string>(row.at("name"))
                  << " (age " << std::get<int>(row.at("age")) << ")\n";
    }

    // Filter
    std::cout << "\nUsers over 28:\n";
    auto adults = db.select("users", "", [](const simple_db::Row& row) {
        return std::get<int>(row.at("age")) > 28;
    });
    for (const auto& row : adults) {
        std::cout << "  " << std::get<std::string>(row.at("name")) << "\n";
    }

    // Update
    std::cout << "\nUpdating Alice's age...\n";
    db.update("users", {{"age", 26}},
        [](const simple_db::Row& row) {
            return std::get<std::string>(row.at("name")) == "Alice";
        });

    // Save
    db.save("/tmp/demo_db.txt");
    std::cout << "Database saved to /tmp/demo_db.txt\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
