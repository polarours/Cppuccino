#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <iostream>

namespace repository_pattern {

class User {
public:
    User(int id, std::string name, std::string email)
        : id_(id), name_(std::move(name)), email_(std::move(email)) {}

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }

    void setName(std::string name) { name_ = std::move(name); }
    void setEmail(std::string email) { email_ = std::move(email); }

private:
    int id_;
    std::string name_;
    std::string email_;
};

class UserRepository {
public:
    void save(const User& user) {
        users_[user.id()] = std::make_shared<User>(user);
    }

    std::shared_ptr<User> findById(int id) {
        auto it = users_.find(id);
        return (it != users_.end()) ? it->second : nullptr;
    }

    std::vector<User> findAll() {
        std::vector<User> result;
        for (const auto& [id, user] : users_) {
            result.push_back(*user);
        }
        return result;
    }

    bool exists(int id) const {
        return users_.find(id) != users_.end();
    }

    void remove(int id) {
        users_.erase(id);
    }

    std::vector<User> findByPredicate(std::function<bool(const User&)> pred) {
        std::vector<User> result;
        for (const auto& [id, user] : users_) {
            if (pred(*user)) {
                result.push_back(*user);
            }
        }
        return result;
    }

    std::size_t count() const { return users_.size(); }

private:
    std::map<int, std::shared_ptr<User>> users_;
};

} // namespace repository_pattern
