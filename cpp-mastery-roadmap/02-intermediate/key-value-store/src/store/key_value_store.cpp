#include "key_value_store.hpp"

#include <utility>
#include <mutex>

namespace key_value_store {

void KeyValueStore::set(std::string key, std::string value) {
    std::unique_lock lock(mutex_);
    data_[std::move(key)] = std::move(value);
}

std::optional<std::string> KeyValueStore::get(const std::string& key) const {
    std::shared_lock lock(mutex_);
    const auto iterator = data_.find(key);
    if (iterator == data_.end()) {
        return std::nullopt;
    }
    return iterator->second;
}

bool KeyValueStore::erase(const std::string& key) {
    std::unique_lock lock(mutex_);
    return data_.erase(key) > 0;
}

bool KeyValueStore::exists(const std::string& key) const {
    std::shared_lock lock(mutex_);
    return data_.find(key) != data_.end();
}

void KeyValueStore::clear() {
    std::unique_lock lock(mutex_);
    data_.clear();
}

std::size_t KeyValueStore::size() const {
    std::shared_lock lock(mutex_);
    return data_.size();
}

std::unordered_map<std::string, std::string> KeyValueStore::snapshot() const {
    std::shared_lock lock(mutex_);
    return data_;
}

void KeyValueStore::replace_all(std::unordered_map<std::string, std::string> data) {
    std::unique_lock lock(mutex_);
    data_ = std::move(data);
}

} // namespace key_value_store
