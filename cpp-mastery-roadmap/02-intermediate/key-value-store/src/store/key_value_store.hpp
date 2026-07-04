#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <shared_mutex>

namespace key_value_store {

class KeyValueStore {
public:
    void set(std::string key, std::string value);
    std::optional<std::string> get(const std::string& key) const;
    bool erase(const std::string& key);
    bool exists(const std::string& key) const;
    void clear();
    std::size_t size() const;

    std::unordered_map<std::string, std::string> snapshot() const;
    void replace_all(std::unordered_map<std::string, std::string> data);

    using const_iterator = std::unordered_map<std::string, std::string>::const_iterator;

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::string> data_{};
};

} // namespace key_value_store
