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
        : state_(std::make_shared<State>(std::move(creator), std::move(deleter),
                                         std::move(validator))),
          minSize_(minSize), maxSize_(maxSize) {
        for (std::size_t i = 0; i < minSize_; ++i) {
            state_->pool_.push(state_->creator_());
        }
    }

    ~ObjectPool() {
        std::lock_guard<std::mutex> lock(state_->mutex_);
        state_->closed_ = true;
        drainLocked();
    }

    std::shared_ptr<T> acquire() {
        // The deleter keeps the shared state alive, so an object checked
        // out of the pool may outlive the pool itself.
        auto state = state_;
        std::lock_guard<std::mutex> lock(state->mutex_);

        T* raw = nullptr;
        while (!state->pool_.empty()) {
            raw = state->pool_.front();
            state->pool_.pop();

            if (state->validator_ && !state->validator_(raw)) {
                state->deleter_(raw);
                raw = nullptr;
                continue;
            }
            break;
        }

        if (!raw) {
            raw = state->creator_();
        }

        ++state->activeCount_;
        return std::shared_ptr<T>(raw, [state](T* p) {
            std::lock_guard<std::mutex> pushLock(state->mutex_);
            --state->activeCount_;
            if (state->closed_) {
                // There is no pool to return to any more.
                state->deleter_(p);
            } else {
                state->pool_.push(p);
            }
        });
    }

    std::size_t poolSize() const {
        std::lock_guard<std::mutex> lock(state_->mutex_);
        return state_->pool_.size();
    }

    std::size_t activeCount() const {
        return state_->activeCount_.load();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(state_->mutex_);
        drainLocked();
    }

private:
    struct State {
        State(Creator c, Deleter d, Validator v)
            : creator_(std::move(c)), deleter_(std::move(d)),
              validator_(std::move(v)) {}

        ~State() { drain(); }

        void drain() {
            while (!pool_.empty()) {
                deleter_(pool_.front());
                pool_.pop();
            }
        }

        Creator creator_;
        Deleter deleter_;
        Validator validator_;
        std::queue<T*> pool_;
        std::atomic<std::size_t> activeCount_{0};
        std::mutex mutex_;
        bool closed_ = false;
    };

    // Called with state_->mutex_ held.
    void drainLocked() { state_->drain(); }

    std::shared_ptr<State> state_;
    std::size_t minSize_;
    std::size_t maxSize_;
};

} // namespace object_pool_advanced
