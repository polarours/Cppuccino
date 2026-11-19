#pragma once

#include <string>
#include <map>
#include <list>
#include <memory>
#include <chrono>
#include <functional>
#include <mutex>
#include <optional>

namespace strategy_cache {

// --- Cache Strategy Interface ---

template <typename Key, typename Value>
class CacheStrategy {
public:
    virtual ~CacheStrategy() = default;
    virtual void put(const Key& key, const Value& value) = 0;
    virtual std::optional<Value> get(const Key& key) = 0;
    virtual void remove(const Key& key) = 0;
    virtual std::size_t size() const = 0;
};

// --- LRU Strategy ---

template <typename Key, typename Value>
class LRUStrategy : public CacheStrategy<Key, Value> {
public:
    explicit LRUStrategy(std::size_t capacity) : capacity_(capacity) {}

    void put(const Key& key, const Value& value) override {
        // A zero-capacity strategy holds nothing. Without this the
        // eviction below would erase end() on an empty map.
        if (capacity_ == 0) return;

        auto it = cache_.find(key);
        if (it != cache_.end()) {
            it->second.first = value;
            order_.splice(order_.begin(), order_, it->second.second);
            return;
        }

        if (cache_.size() >= capacity_) {
            evictOldest();
        }
        auto position = order_.insert(order_.begin(), key);
        cache_.emplace(key, std::make_pair(value, position));
    }

    std::optional<Value> get(const Key& key) override {
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return std::nullopt;
        }
        // A hit counts as a use: the key moves to the front, and the back
        // is what gets evicted.
        order_.splice(order_.begin(), order_, it->second.second);
        return it->second.first;
    }

    void remove(const Key& key) override {
        auto it = cache_.find(key);
        if (it == cache_.end()) return;
        order_.erase(it->second.second);
        cache_.erase(it);
    }

    std::size_t size() const override {
        return cache_.size();
    }

private:
    void evictOldest() {
        if (order_.empty()) return;
        cache_.erase(order_.back());
        order_.pop_back();
    }

    std::size_t capacity_;
    std::map<Key, std::pair<Value, typename std::list<Key>::iterator>> cache_;
    std::list<Key> order_;
};

// --- TTL Strategy ---

template <typename Key, typename Value>
class TTLStrategy : public CacheStrategy<Key, Value> {
public:
    explicit TTLStrategy(std::chrono::seconds ttl) : ttl_(ttl) {}

    void put(const Key& key, const Value& value) override {
        cache_[key] = {value, std::chrono::steady_clock::now()};
    }

    std::optional<Value> get(const Key& key) override {
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return std::nullopt;
        }

        auto elapsed = std::chrono::steady_clock::now() - it->second.second;
        if (elapsed > ttl_) {
            cache_.erase(it);
            return std::nullopt;
        }

        return it->second.first;
    }

    void remove(const Key& key) override {
        cache_.erase(key);
    }

    std::size_t size() const override {
        return cache_.size();
    }

private:
    std::chrono::seconds ttl_;
    std::map<Key, std::pair<Value, std::chrono::steady_clock::time_point>> cache_;
};

// --- Cache Context ---

template <typename Key, typename Value>
class Cache {
public:
    void setStrategy(std::shared_ptr<CacheStrategy<Key, Value>> strategy) {
        strategy_ = strategy;
    }

    void put(const Key& key, const Value& value) {
        if (strategy_) strategy_->put(key, value);
    }

    std::optional<Value> get(const Key& key) {
        return strategy_ ? strategy_->get(key) : std::nullopt;
    }

    void remove(const Key& key) {
        if (strategy_) strategy_->remove(key);
    }

    std::size_t size() const {
        return strategy_ ? strategy_->size() : 0;
    }

private:
    std::shared_ptr<CacheStrategy<Key, Value>> strategy_;
};

} // namespace strategy_cache
