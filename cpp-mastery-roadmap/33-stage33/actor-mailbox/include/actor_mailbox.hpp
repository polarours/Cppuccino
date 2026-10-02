#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace actor {

// Bounded MPMC mailbox + single-threaded actor loop.
// send() is thread-safe from anywhere; the handler runs on ONE thread,
// which is what makes message processing lock-free by design (no locks
// inside the handler).
template <typename Message>
class Mailbox {
public:
    explicit Mailbox(std::size_t bound = 1024) : bound_(bound) {}

    ~Mailbox() { stop(); }

    // Non-blocking send: false when the mailbox is full or stopped.
    bool send(Message msg) {
        std::lock_guard<std::mutex> lock(m_);
        if (stopped_ || queue_.size() >= bound_) return false;
        queue_.push_back(std::move(msg));
        cv_.notify_one();
        return true;
    }

    // Start the actor thread. No-op if already running.
    void start(std::function<void(Message&)> handler) {
        std::lock_guard<std::mutex> lock(m_);
        if (running_) return;
        handler_ = std::move(handler);
        stopped_ = false;
        running_ = true;
        thread_ = std::thread([this] { loop(); });
    }

    // Stop: drain nothing, discard pending, join the actor thread.
    void stop() {
        {
            std::lock_guard<std::mutex> lock(m_);
            if (!running_) return;
            stopped_ = true;
        }
        cv_.notify_all();
        if (thread_.joinable()) thread_.join();
        std::lock_guard<std::mutex> lock(m_);
        running_ = false;
        queue_.clear();
    }

    std::size_t pending() const {
        std::lock_guard<std::mutex> lock(m_);
        return queue_.size();
    }
    std::size_t handled() const { return handled_.load(); }
    std::size_t bound() const { return bound_; }
    bool running() const {
        std::lock_guard<std::mutex> lock(m_);
        return running_;
    }

private:
    void loop() {
        while (true) {
            Message msg;
            {
                std::unique_lock<std::mutex> lock(m_);
                cv_.wait(lock, [this] { return stopped_ || !queue_.empty(); });
                if (stopped_ && queue_.empty()) return;  // graceful: drain first
                if (queue_.empty()) continue;
                msg = std::move(queue_.front());
                queue_.pop_front();
            }
            // handler runs WITHOUT holding the lock
            if (handler_) handler_(msg);
            handled_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    const std::size_t bound_;
    mutable std::mutex m_;
    std::condition_variable cv_;
    std::deque<Message> queue_;
    std::function<void(Message&)> handler_;
    bool stopped_ = false;
    bool running_ = false;
    std::atomic<std::size_t> handled_{0};
    std::thread thread_;
};

}  // namespace actor
