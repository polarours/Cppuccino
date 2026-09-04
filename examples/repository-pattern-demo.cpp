// examples/repository-pattern-demo.cpp
// Demonstrates Repository Pattern with User entity and in-memory repository.
// Compile: g++ -std=c++20 -o repository-pattern-demo repository-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace repository_pattern {

class User {
public:
    User(int id, std::string name, std::string email)
        : id_(id), name_(std::move(name)), email_(std::move(email)) {}
    int id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }
    void setName(std::string n) { name_ = std::move(n); }
    void setEmail(std::string e) { email_ = std::move(e); }
private:
    int id_;
    std::string name_;
    std::string email_;
};

class UserRepository {
public:
    void save(const User& u) { users_[u.id()] = std::make_shared<User>(u); }
    std::shared_ptr<User> findById(int id) {
        auto it = users_.find(id);
        return (it != users_.end()) ? it->second : nullptr;
    }
    std::vector<User> findAll() {
        std::vector<User> r;
        for (const auto& [_, u] : users_) r.push_back(*u);
        return r;
    }
    bool exists(int id) const { return users_.find(id) != users_.end(); }
    void remove(int id) { users_.erase(id); }
    std::vector<User> findByPredicate(std::function<bool(const User&)> pred) {
        std::vector<User> r;
        for (const auto& [_, u] : users_)
            if (pred(*u)) r.push_back(*u);
        return r;
    }
    std::size_t count() const { return users_.size(); }
private:
    std::map<int, std::shared_ptr<User>> users_;
};

} // namespace repository_pattern

int main() {
    using namespace repository_pattern;
    std::cout << "=== Repository Pattern Demo ===\n\n";

    UserRepository repo;
    repo.save(User(1, "Alice", "alice@example.com"));
    repo.save(User(2, "Bob",   "bob@example.com"));
    repo.save(User(3, "Carol", "carol@example.com"));

    auto u = repo.findById(2);
    std::cout << "Find by ID=2: " << u->name() << " <" << u->email() << ">\n";

    auto devs = repo.findByPredicate([](const User& u){
        return u.email().find("example") != std::string::npos;
    });
    std::cout << "Total users: " << devs.size() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
