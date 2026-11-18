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
        std::unique_lock lock(mutex_);

        auto it = data_.find(key);
        if (it == data_.end()) {
            return std::nullopt;
        }

        // Check expiration
        if (isExpired(it->second)) {
            order_.erase(it->second.orderPos);
            data_.erase(it);
            return std::nullopt;
        }

        // A hit makes this the most recently used key, so the read also
        // takes the write lock.
        order_.splice(order_.begin(), order_, it->second.orderPos);
        return it->second.value;
    }

    void set(const Key& key, Value value) {
        std::unique_lock lock(mutex_);

        // A zero-capacity cache cannot hold anything. Without this, the
        // eviction finds nothing to remove and an entry is stored anyway,
        // even though capacity() reports zero.
        if (maxSize_ == 0) {
            return;
        }

        setLocked(key, std::move(value), std::chrono::milliseconds{0});
    }

    void set(const Key& key, Value value, std::chrono::milliseconds ttl) {
        std::unique_lock lock(mutex_);

        if (maxSize_ == 0) {
            return;
        }

        setLocked(key, std::move(value), ttl);
    }

    bool erase(const Key& key) {
        std::unique_lock lock(mutex_);
        auto it = data_.find(key);
        if (it == data_.end()) {
            return false;
        }
        order_.erase(it->second.orderPos);
        data_.erase(it);
        return true;
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
        order_.clear();
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
        typename std::list<Key>::iterator orderPos;
    };

    bool isExpired(const Entry& entry) const {
        if (entry.ttl.count() == 0) return false;
        auto now = std::chrono::steady_clock::now();
        return (now - entry.createdAt) > entry.ttl;
    }

    // Called with the write lock held. The front of order_ is the most
    // recently used key, so the back is the one to evict.
    void setLocked(const Key& key, Value value, std::chrono::milliseconds ttl) {
        auto existing = data_.find(key);
        if (existing != data_.end()) {
            existing->second.value = std::move(value);
            existing->second.createdAt = std::chrono::steady_clock::now();
            existing->second.ttl = ttl;
            order_.splice(order_.begin(), order_, existing->second.orderPos);
            return;
        }

        if (data_.size() >= maxSize_) {
            evictOldest();
        }

        auto position = order_.insert(order_.begin(), key);
        data_.emplace(key, Entry{std::move(value), std::chrono::steady_clock::now(), ttl, position});
    }

    void evictOldest() {
        if (order_.empty()) {
            return;
        }
        const Key victim = order_.back();
        data_.erase(victim);
        order_.pop_back();
    }

    std::size_t maxSize_;
    std::map<Key, Entry> data_;
    std::list<Key> order_;
    mutable std::shared_mutex mutex_;
};

} // namespace thread_safe_cache
