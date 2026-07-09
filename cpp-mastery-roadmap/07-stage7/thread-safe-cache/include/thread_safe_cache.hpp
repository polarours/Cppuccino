#pragma once

#include <map>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <chrono>
#include <optional>
#include <list>

namespace thread_safe_cache {

template <typename Key, typename Value>
class Cache {
public:
    explicit Cache(std::size_t maxSize = 1000)
        : maxSize_(maxSize) {}

    std::optional<Value> get(const Key& key) {
        std::shared_lock lock(mutex_);

        auto it = data_.find(key);
        if (it == data_.end()) {
            return std::nullopt;
        }

        // Check expiration
        if (isExpired(it->second)) {
            return std::nullopt;
        }

        return it->second.value;
    }

    void set(const Key& key, Value value) {
        std::unique_lock lock(mutex_);

        // Evict if at capacity
        if (data_.size() >= maxSize_ && data_.find(key) == data_.end()) {
            evictOldest();
        }

        data_[key] = Entry{std::move(value), std::chrono::steady_clock::now()};
    }

    void set(const Key& key, Value value, std::chrono::milliseconds ttl) {
        std::unique_lock lock(mutex_);

        if (data_.size() >= maxSize_ && data_.find(key) == data_.end()) {
            evictOldest();
        }

        data_[key] = Entry{std::move(value), std::chrono::steady_clock::now(), ttl};
    }

    bool erase(const Key& key) {
        std::unique_lock lock(mutex_);
        return data_.erase(key) > 0;
    }

    bool contains(const Key& key) const {
        std::shared_lock lock(mutex_);
        auto it = data_.find(key);
        if (it == data_.end()) return false;
        return !isExpired(it->second);
    }

    void clear() {
        std::unique_lock lock(mutex_);
        data_.clear();
    }

    std::size_t size() const {
        std::shared_lock lock(mutex_);
        return data_.size();
    }

    std::size_t capacity() const {
        return maxSize_;
    }

    Value getOrElse(const Key& key, const Value& defaultValue) {
        auto result = get(key);
        return result.value_or(defaultValue);
    }

    Value getOrCreate(const Key& key, std::function<Value()> creator) {
        auto result = get(key);
        if (result) {
            return *result;
        }

        Value value = creator();
        set(key, value);
        return value;
    }

private:
    struct Entry {
        Value value;
        std::chrono::steady_clock::time_point createdAt;
        std::chrono::milliseconds ttl{0};
    };

    bool isExpired(const Entry& entry) const {
        if (entry.ttl.count() == 0) return false;
        auto now = std::chrono::steady_clock::now();
        return (now - entry.createdAt) > entry.ttl;
    }

    void evictOldest() {
        if (!data_.empty()) {
            data_.erase(data_.begin());
        }
    }

    std::size_t maxSize_;
    std::map<Key, Entry> data_;
    mutable std::shared_mutex mutex_;
};

} // namespace thread_safe_cache
