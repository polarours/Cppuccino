#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <functional>
#include <memory>
#include <stdexcept>

namespace object_pool {

template <typename T>
class ObjectPool {
public:
    using Creator = std::function<T*()>;
    using Deleter = std::function<void(T*)>;

    ObjectPool(Creator creator, Deleter deleter, std::size_t initialSize = 0)
        : creator_(std::move(creator))
        , deleter_(std::move(deleter)) {
        for (std::size_t i = 0; i < initialSize; ++i) {
            pool_.push(creator_());
        }
    }

    ~ObjectPool() {
        clear();
    }

    std::shared_ptr<T> acquire() {
        std::lock_guard<std::mutex> lock(mutex_);

        T* raw = nullptr;
        if (!pool_.empty()) {
            raw = pool_.front();
            pool_.pop();
        } else {
            raw = creator_();
        }

        return std::shared_ptr<T>(raw, [this](T* p) {
            std::lock_guard<std::mutex> lock(mutex_);
            pool_.push(p);
        });
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
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
    std::queue<T*> pool_;
    mutable std::mutex mutex_;
};

} // namespace object_pool
