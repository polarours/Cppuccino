#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

namespace object_pool {

template <typename T>
class ObjectPool {
public:
    using Creator = std::function<T*()>;
    using Deleter = std::function<void(T*)>;

    ObjectPool(Creator creator, Deleter deleter, std::size_t initialSize = 0)
        : state_(std::make_shared<State>(std::move(creator), std::move(deleter))) {
        for (std::size_t i = 0; i < initialSize; ++i) {
            state_->pool.push(state_->creator_());
        }
    }

    ~ObjectPool() {
        std::lock_guard<std::mutex> lock(state_->mutex);
        state_->closed = true;
        drainLocked();
    }

    std::shared_ptr<T> acquire() {
        // The deleter of the returned pointer keeps the shared state alive
        // even when the pool itself is already gone.
        auto state = state_;
        T* raw = nullptr;

        std::lock_guard<std::mutex> lock(state->mutex);
        if (!state->closed && !state->pool.empty()) {
            raw = state->pool.front();
            state->pool.pop();
        } else {
            raw = state->creator_();
        }

        return std::shared_ptr<T>(raw, [state](T* p) {
            std::lock_guard<std::mutex> pushLock(state->mutex);
            if (state->closed) {
                // There is no pool to return to any more.
                state->deleter_(p);
            } else {
                state->pool.push(p);
            }
        });
    }

    std::size_t size() const {
        std::lock_guard<std::mutex> lock(state_->mutex);
        return state_->pool.size();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(state_->mutex);
        drainLocked();
    }

private:
    struct State {
        State(Creator c, Deleter d)
            : creator_(std::move(c)), deleter_(std::move(d)) {}

        ~State() {
            while (!pool.empty()) {
                deleter_(pool.front());
                pool.pop();
            }
        }

        Creator creator_;
        Deleter deleter_;
        std::queue<T*> pool;
        std::mutex mutex;
        bool closed = false;
    };

    // Called with state_->mutex held.
    void drainLocked() {
        while (!state_->pool.empty()) {
            state_->deleter_(state_->pool.front());
            state_->pool.pop();
        }
    }

    std::shared_ptr<State> state_;
};

} // namespace object_pool
