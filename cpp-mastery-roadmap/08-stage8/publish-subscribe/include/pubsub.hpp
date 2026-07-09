#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm>

namespace pubsub {

template <typename Message>
class Publisher {
public:
    using Callback = std::function<void(const Message&)>;

    int subscribe(const std::string& topic, Callback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        int id = nextId_++;
        subscribers_[topic].push_back({id, std::move(callback)});
        return id;
    }

    void unsubscribe(int id) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& [topic, subs] : subscribers_) {
            auto it = std::remove_if(subs.begin(), subs.end(),
                [id](const Subscriber& s) { return s.id == id; });
            if (it != subs.end()) {
                subs.erase(it, subs.end());
                return;
            }
        }
    }

    void publish(const std::string& topic, const Message& message) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = subscribers_.find(topic);
        if (it != subscribers_.end()) {
            for (const auto& sub : it->second) {
                sub.callback(message);
            }
        }
    }

    std::size_t subscriberCount(const std::string& topic) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = subscribers_.find(topic);
        return (it != subscribers_.end()) ? it->second.size() : 0;
    }

    std::size_t totalSubscribers() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::size_t count = 0;
        for (const auto& [topic, subs] : subscribers_) {
            count += subs.size();
        }
        return count;
    }

private:
    struct Subscriber {
        int id;
        Callback callback;
    };

    std::map<std::string, std::vector<Subscriber>> subscribers_;
    int nextId_{1};
    mutable std::mutex mutex_;
};

} // namespace pubsub
