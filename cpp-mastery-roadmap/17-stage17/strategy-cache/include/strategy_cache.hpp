#pragma once

#include <string>
#include <map>
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
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            it->second = value;
            return;
        }

        if (cache_.size() >= capacity_) {
            cache_.erase(cache_.begin());
        }
        cache_[key] = value;
    }

    std::optional<Value> get(const Key& key) override {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void remove(const Key& key) override {
        cache_.erase(key);
    }

    std::size_t size() const override {
        return cache_.size();
    }

private:
    std::size_t capacity_;
    std::map<Key, Value> cache_;
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
