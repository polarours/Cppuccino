#pragma once

#include <cstddef>
#include <list>
#include <optional>
#include <unordered_map>

namespace lfu_cache {

// Least Frequently Used cache with O(1) get/put.
// Tie-break among equal frequencies: least recently used evicted first.
template <typename K, typename V>
class LFUCache {
public:
    explicit LFUCache(std::size_t capacity) : capacity_(capacity) {}

    std::optional<V> get(const K& key) {
        auto it = index_.find(key);
        if (it == index_.end()) return std::nullopt;
        touch(it);
        return it->second->value;
    }

    void put(const K& key, const V& value) {
        if (capacity_ == 0) return;
        auto it = index_.find(key);
        if (it != index_.end()) {
            it->second->value = value;
            touch(it);
            return;
        }
        if (index_.size() >= capacity_) evict();
        // insert at frequency 1, front of that frequency's LRU list
        auto& bucket = freq_[1];
        bucket.push_front({key, value, 1});
        index_[key] = bucket.begin();
        minFreq_ = 1;
    }

    std::size_t size() const { return index_.size(); }
    std::size_t capacity() const { return capacity_; }

private:
    struct Node {
        K key;
        V value;
        std::size_t freq;
    };
    using Bucket = std::list<Node>;
    using It = typename Bucket::iterator;

    void touch(typename std::unordered_map<K, It>::iterator it) {
        It node = it->second;
        const std::size_t f = node->freq;
        auto fromBucket = freq_.find(f);
        auto& toBucket = freq_[f + 1];  // may default-insert
        // Move the node across buckets, then bump its frequency.
        toBucket.splice(toBucket.begin(), fromBucket->second, node);
        node->freq = f + 1;
        if (fromBucket->second.empty()) {
            freq_.erase(fromBucket);
            if (minFreq_ == f) ++minFreq_;  // no node left at the old minimum
        }
    }

    void evict() {
        auto bucketIt = freq_.find(minFreq_);
        if (bucketIt == freq_.end() || bucketIt->second.empty()) return;
        Node victim = bucketIt->second.back();
        bucketIt->second.pop_back();
        index_.erase(victim.key);
        if (bucketIt->second.empty()) freq_.erase(bucketIt);
    }

    std::size_t capacity_;
    std::size_t minFreq_ = 0;
    std::unordered_map<K, It> index_;
    std::unordered_map<std::size_t, Bucket> freq_;
};

}  // namespace lfu_cache
