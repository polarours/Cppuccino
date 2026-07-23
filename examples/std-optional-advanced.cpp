#include <optional>
#include <string>
#include <iostream>
#include <map>

// std::optional - represents optional values (C++17)
// <optional>

struct User {
    int id;
    std::string name;
    std::string email;
};

class UserDatabase {
public:
    void addUser(int id, User user) {
        users_[id] = std::move(user);
    }

    std::optional<User> findUser(int id) const {
        auto it = users_.find(id);
        if (it != users_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<std::string> getEmail(int id) const {
        if (auto user = findUser(id)) {
            return user->email;
        }
        return std::nullopt;
    }

private:
    std::map<int, User> users_;
};

std::optional<int> divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

int main() {
    std::cout << "=== std::optional (C++17) ===\n\n";

    // Basic usage
    std::optional<int> opt1 = 42;
    std::optional<int> opt2 = std::nullopt;

    std::cout << "opt1 has value: " << opt1.has_value() << "\n";  // true
    std::cout << "opt1 value: " << *opt1 << "\n";  // 42
    std::cout << "opt2 has value: " << opt2.has_value() << "\n";  // false

    // value_or
    std::cout << "opt2 value_or(0): " << opt2.value_or(0) << "\n";  // 0

    // Optional with functions
    auto result = divide(10, 3);
    if (result) {
        std::cout << "\n10/3 = " << *result << "\n";
    }

    auto error = divide(10, 0);
    if (!error) {
        std::cout << "10/0: division by zero\n";
    }

    // Optional in data structures
    UserDatabase db;
    db.addUser(1, {1, "Alice", "alice@example.com"});
    db.addUser(2, {2, "Bob", "bob@example.com"});

    if (auto user = db.findUser(1)) {
        std::cout << "\nFound user: " << user->name << "\n";
    }

    if (auto email = db.getEmail(1)) {
        std::cout << "Email: " << *email << "\n";
    }

    if (!db.findUser(99)) {
        std::cout << "User 99 not found\n";
    }

    return 0;
}
