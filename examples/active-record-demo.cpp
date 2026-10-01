// examples/active-record-demo.cpp
// Demonstrates the Active Record Pattern: each object knows how to persist
// itself (object + row + data access in one class).
// Compile: g++ -std=c++20 -Wall -Wextra -o active-record-demo active-record-demo.cpp

#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace active_record {

// Fake in-memory table shared by all records
class Table {
public:
    static Table& users() {
        static Table t;
        return t;
    }

    void put(int id, const std::map<std::string, std::string>& row) { rows_[id] = row; }
    std::optional<std::map<std::string, std::string>> get(int id) const {
        auto it = rows_.find(id);
        if (it == rows_.end()) return std::nullopt;
        return it->second;
    }
    void erase(int id) { rows_.erase(id); }
    void clear() { rows_.clear(); }
    size_t count() const { return rows_.size(); }

private:
    std::map<int, std::map<std::string, std::string>> rows_;
};

// Active Record: domain logic + persistence in the same class
class User {
public:
    User(std::string name, std::string email)
        : name_(std::move(name)), email_(std::move(email)) {}

    static User find(int id) {
        auto row = Table::users().get(id);
        if (!row) throw std::runtime_error("User not found: " + std::to_string(id));
        User u((*row)["name"], (*row)["email"]);
        u.id_ = id;
        u.persisted_ = true;
        return u;
    }

    static std::vector<User> all() {
        std::vector<User> out;
        // Demo-scale: re-read known ids through find()
        for (int id = 1; id <= 1000; ++id) {
            if (auto row = Table::users().get(id)) {
                User u((*row)["name"], (*row)["email"]);
                u.id_ = id;
                u.persisted_ = true;
                out.push_back(std::move(u));
            }
        }
        return out;
    }

    // insert or update depending on whether this row already exists
    void save() {
        if (!persisted_) {
            id_ = nextId();
            persisted_ = true;
        }
        Table::users().put(id_, {{"name", name_}, {"email", email_}});
        std::cout << "  saved User{id=" << id_ << ", name=" << name_ << "}\n";
    }

    void destroy() {
        if (persisted_) {
            Table::users().erase(id_);
            std::cout << "  destroyed User{id=" << id_ << "}\n";
        }
    }

    // Domain logic lives next to persistence
    bool isCorporateEmail() const {
        return email_.find("@company.com") != std::string::npos;
    }

    void rename(const std::string& newName) { name_ = newName; }

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }

private:
    static int nextId() { static int n = 0; return ++n; }

    int id_ = -1;
    std::string name_;
    std::string email_;
    bool persisted_ = false;
};

} // namespace active_record

int main() {
    using namespace active_record;
    std::cout << "=== Active Record Demo ===\n\n";

    Table::users().clear();

    User alice("alice", "alice@company.com");
    User bob("bob", "bob@example.com");

    std::cout << "Creating:\n";
    alice.save();
    bob.save();

    std::cout << "\nRound-trip via find():\n";
    User loaded = User::find(alice.id());
    std::cout << "  loaded: " << loaded.name() << " <" << loaded.email() << ">\n";

    std::cout << "\nUpdate (rename + save):\n";
    loaded.rename("alice.w");
    loaded.save();

    std::cout << "\nDomain logic on the record:\n";
    std::cout << "  alice corporate? " << std::boolalpha
              << loaded.isCorporateEmail() << "\n";
    std::cout << "  bob corporate?   " << bob.isCorporateEmail() << "\n";

    std::cout << "\nDelete bob:\n";
    bob.destroy();
    std::cout << "rows remaining: " << Table::users().count() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
