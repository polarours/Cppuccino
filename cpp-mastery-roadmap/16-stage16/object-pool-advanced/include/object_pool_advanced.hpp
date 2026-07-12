#pragma once

#include <memory>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <chrono>

namespace object_pool_advanced {

template <typename T>
class ObjectPool {
public:
    using Creator = std::function<T*()>;
    using Deleter = std::function<void(T*)>;
    using Validator = std::function<bool(const T*)>;

    ObjectPool(Creator creator, Deleter deleter, Validator validator,
               std::size_t minSize, std::size_t maxSize)
        : creator_(std::move(creator))
        , deleter_(std::move(deleter))
        , validator_(std::move(validator))
        , minSize_(minSize)
        , maxSize_(maxSize) {
        for (std::size_t i = 0; i < minSize; ++i) {
            pool_.push(creator_());
        }
    }

    ~ObjectPool() {
        clear();
    }

    std::shared_ptr<T> acquire() {
        std::lock_guard<std::mutex> lock(mutex_);

        T* raw = nullptr;

        // Try to get from pool
        while (!pool_.empty()) {
            raw = pool_.front();
            pool_.pop();

            // Validate before returning
            if (validator_ && !validator_(raw)) {
                deleter_(raw);
                raw = nullptr;
                continue;
            }
            break;
        }

        // Create new if pool empty
        if (!raw) {
            raw = creator_();
        }

        return std::shared_ptr<T>(raw, [this](T* p) {
            std::lock_guard<std::mutex> lock(mutex_);
            pool_.push(p);
        });
    }

    std::size_t poolSize() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
    }

    std::size_t activeCount() const {
        return activeCount_;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!pool_.empty()) {
            deleter_(pool_.front());
            pool_.pop();
        }
    }

private:
    Creator creator_;
    Deleter deleter_;
    Validator validator_;
    std::size_t minSize_;
    std::size_t maxSize_;
    std::queue<T*> pool_;
    std::atomic<std::size_t> activeCount_{0};
    mutable std::mutex mutex_;
};

} // namespace object_pool_advanced
