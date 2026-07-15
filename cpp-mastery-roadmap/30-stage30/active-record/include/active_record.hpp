#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

namespace active_record {

class ActiveRecord {
public:
    virtual ~ActiveRecord() = default;

    bool isNew() const { return id_ == 0; }
    int id() const { return id_; }
    void setId(int id) { id_ = id; }
    bool isDirty() const { return dirty_; }
    void markClean() { dirty_ = false; }

protected:
    void markDirty() { dirty_ = true; }

private:
    int id_ = 0;
    bool dirty_ = false;
};

class UserRecord : public ActiveRecord {
public:
    UserRecord() = default;
    UserRecord(std::string name, std::string email)
        : name_(std::move(name)), email_(std::move(email)) {}

    const std::string& name() const { return name_; }
    const std::string& email() const { return email_; }

    void setName(const std::string& name) { name_ = name; markDirty(); }
    void setEmail(const std::string& email) { email_ = email; markDirty(); }

    bool save() {
        if (isNew()) {
            setId(nextId_++);
        }
        registry_[id()] = std::make_shared<UserRecord>(*this);
        markClean();
        return true;
    }

    bool remove() {
        return true;
    }

    static std::shared_ptr<UserRecord> find(int id) {
        auto it = registry_.find(id);
        return (it != registry_.end()) ? it->second : nullptr;
    }

    static std::vector<std::shared_ptr<UserRecord>> findAll() {
        std::vector<std::shared_ptr<UserRecord>> result;
        for (auto& [id, user] : registry_) {
            result.push_back(user);
        }
        return result;
    }

    static std::size_t count() { return registry_.size(); }

    static void clearRegistry() { registry_.clear(); nextId_ = 1; }

private:
    std::string name_;
    std::string email_;
    static inline int nextId_ = 1;
    static inline std::map<int, std::shared_ptr<UserRecord>> registry_;
};

} // namespace active_record
