#pragma once

#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <stdexcept>

namespace lru_cache {

template <typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(std::size_t capacity)
        : capacity_(capacity) {}

    std::optional<Value> get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = map_.find(key);
        if (it == map_.end()) {
            return std::nullopt;
        }

        // Move to front (most recently used)
        order_.splice(order_.begin(), order_, it->second);
        return it->second->second;
    }

    void put(const Key& key, Value value) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = map_.find(key);
        if (it != map_.end()) {
            // Update existing entry
            it->second->second = std::move(value);
            order_.splice(order_.begin(), order_, it->second);
            return;
        }

        // Evict if at capacity
        if (map_.size() >= capacity_) {
            auto last = order_.back();
            map_.erase(last.first);
            order_.pop_back();
        }

        // Add new entry
        order_.emplace_front(key, std::move(value));
        map_[key] = order_.begin();
    }

    bool erase(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = map_.find(key);
        if (it == map_.end()) {
            return false;
        }

        order_.erase(it->second);
        map_.erase(it);
        return true;
    }

    bool contains(const Key& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.find(key) != map_.end();
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.size();
    }

    std::size_t capacity() const {
        return capacity_;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        map_.clear();
        order_.clear();
    }

    Value getOrElse(const Key& key, const Value& defaultValue) {
        auto result = get(key);
        return result.value_or(defaultValue);
    }

private:
    using ListIterator = typename std::list<std::pair<Key, Value>>::iterator;

    std::size_t capacity_;
    std::list<std::pair<Key, Value>> order_;
    std::map<Key, ListIterator> map_;
    mutable std::mutex mutex_;
};

} // namespace lru_cache
