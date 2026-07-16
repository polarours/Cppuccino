#pragma once

#include <shared_mutex>
#include <mutex>
#include <map>
#include <string>
#include <vector>
#include <iostream>

namespace read_write_lock {

template <typename K, typename V>
class ThreadSafeMap {
public:
    void insert(const K& key, const V& value) {
        std::unique_lock lock(mutex_);
        data_[key] = value;
    }

    V get(const K& key) const {
        std::shared_lock lock(mutex_);
        auto it = data_.find(key);
        return (it != data_.end()) ? it->second : V{};
    }

    bool contains(const K& key) const {
        std::shared_lock lock(mutex_);
        return data_.find(key) != data_.end();
    }

    bool erase(const K& key) {
        std::unique_lock lock(mutex_);
        return data_.erase(key) > 0;
    }

    std::size_t size() const {
        std::shared_lock lock(mutex_);
        return data_.size();
    }

    std::vector<K> keys() const {
        std::shared_lock lock(mutex_);
        std::vector<K> result;
        for (const auto& [k, v] : data_) {
            result.push_back(k);
        }
        return result;
    }

private:
    mutable std::shared_mutex mutex_;
    std::map<K, V> data_;
};

} // namespace read_write_lock
