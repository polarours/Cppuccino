// examples/std-optional.cpp
// Demonstrates std::optional usage and patterns.
// Compile with: g++ -std=c++17 -o std_optional std-optional.cpp

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <map>

// --- 1. Basic Usage ---

void basicUsage() {
    std::cout << "--- Basic Usage ---\n";

    std::optional<int> opt1;                    // Empty
    std::optional<int> opt2 = 42;              // Has value
    std::optional<std::string> opt3 = "hello"; // Has value

    std::cout << "opt1 has value: " << opt1.has_value() << "\n";
    std::cout << "opt2 has value: " << opt2.has_value() << "\n";
    std::cout << "opt2 value: " << opt2.value() << "\n";
    std::cout << "opt3 value: " << opt3.value() << "\n";
}

// --- 2. Value or Default ---

void valueOrDefault() {
    std::cout << "\n--- Value or Default ---\n";

    std::optional<int> empty;
    std::optional<int> present = 42;

    std::cout << "empty.value_or(0): " << empty.value_or(0) << "\n";
    std::cout << "present.value_or(0): " << present.value_or(0) << "\n";

    std::optional<std::string> name;
    std::cout << "name.value_or(\"Unknown\"): " << name.value_or("Unknown") << "\n";

    name = "Alice";
    std::cout << "name.value_or(\"Unknown\"): " << name.value_or("Unknown") << "\n";
}

// --- 3. Reset and Emplace ---

void resetAndEmplace() {
    std::cout << "\n--- Reset and Emplace ---\n";

    std::optional<int> opt = 42;
    std::cout << "Before reset: " << opt.value_or(-1) << "\n";

    opt.reset();
    std::cout << "After reset: " << opt.value_or(-1) << "\n";

    opt.emplace(100);
    std::cout << "After emplace: " << opt.value() << "\n";
}

// --- 4. Conversions ---

void conversions() {
    std::cout << "\n--- Conversions ---\n";

    std::optional<bool> optTrue = true;
    std::optional<bool> optFalse = false;
    std::optional<bool> optEmpty;

    // Be careful with optional<bool>!
    if (optTrue) {
        std::cout << "optTrue has value\n";
    }

    if (optFalse) {
        std::cout << "optFalse has value\n";
    }

    if (!optEmpty) {
        std::cout << "optEmpty is empty\n";
    }

    std::cout << "optTrue.value(): " << std::boolalpha << optTrue.value() << "\n";
    std::cout << "optFalse.value(): " << optFalse.value() << "\n";
}

// --- 5. Function Return Values ---

std::optional<int> findIndex(const std::vector<int>& vec, int target) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == target) {
            return static_cast<int>(i);
        }
    }
    return std::nullopt;
}

std::optional<std::string> findName(const std::map<int, std::string>& map, int key) {
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    }
    return std::nullopt;
}

void functionReturns() {
    std::cout << "\n--- Function Return Values ---\n";

    std::vector<int> nums = {10, 20, 30, 40, 50};

    if (auto idx = findIndex(nums, 30)) {
        std::cout << "Found 30 at index: " << *idx << "\n";
    }

    if (auto idx = findIndex(nums, 99)) {
        std::cout << "Found 99 at index: " << *idx << "\n";
    } else {
        std::cout << "99 not found\n";
    }

    std::map<int, std::string> names = {{1, "Alice"}, {2, "Bob"}, {3, "Charlie"}};

    if (auto name = findName(names, 2)) {
        std::cout << "Name for key 2: " << *name << "\n";
    }
}

// --- 6. Chain of Operations ---

std::optional<int> parseAge(const std::string& str) {
    try {
        int age = std::stoi(str);
        if (age >= 0 && age <= 150) {
            return age;
        }
    } catch (...) {}
    return std::nullopt;
}

std::optional<bool> isAdult(int age) {
    return age >= 18;
}

void chainOfOperations() {
    std::cout << "\n--- Chain of Operations ---\n";

    std::vector<std::string> inputs = {"25", "abc", "17", "200", "30"};

    for (const auto& input : inputs) {
        std::cout << "Input: \"" << input << "\" -> ";

        if (auto age = parseAge(input)) {
            if (auto adult = isAdult(*age)) {
                std::cout << "Age " << *age << ", Adult: " << std::boolalpha << *adult;
            }
        } else {
            std::cout << "Invalid";
        }
        std::cout << "\n";
    }
}

// --- 7. Optional with Struct ---

struct Config {
    std::string host;
    int port;
    bool useSsl;
};

std::optional<Config> loadConfig(const std::string& filename) {
    if (filename.empty()) {
        return std::nullopt;
    }

    return Config{"localhost", 8080, false};
}

void optionalWithStruct() {
    std::cout << "\n--- Optional with Struct ---\n";

    if (auto config = loadConfig("config.txt")) {
        std::cout << "Host: " << config->host << "\n";
        std::cout << "Port: " << config->port << "\n";
        std::cout << "SSL: " << std::boolalpha << config->useSsl << "\n";
    }

    if (auto config = loadConfig("")) {
        std::cout << "Loaded config\n";
    } else {
        std::cout << "No config file\n";
    }
}

// --- 8. Optional Comparison ---

void optionalComparison() {
    std::cout << "\n--- Optional Comparison ---\n";

    std::optional<int> a = 10;
    std::optional<int> b = 20;
    std::optional<int> c;
    std::optional<int> d = 10;

    std::cout << "a == d: " << (a == d) << "\n";
    std::cout << "a < b: " << (a < b) << "\n";
    std::cout << "c < a: " << (c < a) << "\n";
    std::cout << "a == std::nullopt: " << (a == std::nullopt) << "\n";
    std::cout << "c == std::nullopt: " << (c == std::nullopt) << "\n";
}

// --- 9. Optional in Class ---

class UserDatabase {
public:
    void addUser(int id, const std::string& name) {
        users_[id] = name;
    }

    std::optional<std::string> findUser(int id) const {
        auto it = users_.find(id);
        if (it != users_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool removeUser(int id) {
        return users_.erase(id) > 0;
    }

private:
    std::map<int, std::string> users_;
};

void optionalInClass() {
    std::cout << "\n--- Optional in Class ---\n";

    UserDatabase db;
    db.addUser(1, "Alice");
    db.addUser(2, "Bob");

    if (auto user = db.findUser(1)) {
        std::cout << "User 1: " << *user << "\n";
    }

    if (auto user = db.findUser(99)) {
        std::cout << "User 99: " << *user << "\n";
    } else {
        std::cout << "User 99 not found\n";
    }
}

int main() {
    basicUsage();
    valueOrDefault();
    resetAndEmplace();
    conversions();
    functionReturns();
    chainOfOperations();
    optionalWithStruct();
    optionalComparison();
    optionalInClass();

    std::cout << "\n=== All optional scenarios completed ===\n";
    return 0;
}
