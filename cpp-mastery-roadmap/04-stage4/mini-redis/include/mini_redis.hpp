#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace mini_redis {

using Clock = std::chrono::steady_clock;

// In-memory KV store implementing a small Redis-like command set:
//   SET key value [SECONDS]   - store string, optional TTL in seconds
//   GET key                   - bulk string or null
//   DEL key                   - remove, reports if existed
//   INCR key                  - integer increment (must be integer or missing)
//   EXPIRE key SECONDS        - set TTL on existing key
//   TTL key                   - remaining seconds: -2 no key, -1 no ttl
//   DBSIZE                    - number of live keys (expired excluded)
// TTL is checked lazily on access and by purge() (like Redis: expiry on
// access is always correct; background purge is opportunistic).
class Store {
public:
    explicit Store(Clock::duration tick = std::chrono::milliseconds(100))
        : tick_(tick) {}

    // Execute one command line (whitespace-split, no quoted strings
    // - quoted parsing is a documented non-goal). Returns RESP-ish lines:
    // "$n\r\n<payload>\r\n", ":n\r\n", "-ERR ...\r\n", "$-1\r\n".
    std::string execute(const std::string& commandLine);

    // Direct API (used by tests and by execute()).
    bool set(const std::string& key, const std::string& value,
             std::optional<std::chrono::seconds> ttl, Clock::time_point now);
    std::optional<std::string> get(const std::string& key, Clock::time_point now);
    bool del(const std::string& key, Clock::time_point now);
    std::optional<long long> incr(const std::string& key, Clock::time_point now);
    bool expire(const std::string& key, std::chrono::seconds ttl,
                Clock::time_point now);
    // -2: no key; -1: no ttl; else remaining seconds (rounded up)
    long long ttlSeconds(const std::string& key, Clock::time_point now) const;
    std::size_t dbSize(Clock::time_point now) const;
    void purge(Clock::time_point now);

    // Periodic purge: call regularly from a driver; every `tick_` elapsed.
    void maybePurge(Clock::time_point now);

private:
    struct Entry {
        std::string value;
        std::optional<Clock::time_point> expiresAt;
    };

    static bool expired(const Entry& e, Clock::time_point now) {
        return e.expiresAt && *e.expiresAt <= now;
    }

    std::unordered_map<std::string, Entry> kv_;
    Clock::duration tick_;
    Clock::time_point lastPurge_ = Clock::now();
};

// Parse helpers exposed for tests.
std::vector<std::string> splitArgs(const std::string& line);

}  // namespace mini_redis
