#include "mini_redis.hpp"

#include <atomic>
#include <cstdlib>
#include <sstream>

namespace mini_redis {

std::vector<std::string> splitArgs(const std::string& line) {
    std::vector<std::string> out;
    std::istringstream in(line);
    std::string tok;
    while (in >> tok) out.push_back(tok);
    return out;
}

namespace {

std::string bulk(const std::string& s) {
    return "$" + std::to_string(s.size()) + "\r\n" + s + "\r\n";
}
std::string integer(long long v) {
    return ":" + std::to_string(v) + "\r\n";
}
std::string nullBulk() { return "$-1\r\n"; }
std::string error(const std::string& msg) { return "-ERR " + msg + "\r\n"; }

}  // namespace

bool Store::set(const std::string& key, const std::string& value,
                std::optional<std::chrono::seconds> ttl, Clock::time_point now) {
    Entry e;
    e.value = value;
    if (ttl && ttl->count() > 0) e.expiresAt = now + *ttl;
    kv_[key] = std::move(e);
    purge(now);  // opportunistic cleanup on write
    return true;
}

std::optional<std::string> Store::get(const std::string& key, Clock::time_point now) {
    auto it = kv_.find(key);
    if (it == kv_.end()) return std::nullopt;
    if (expired(it->second, now)) { kv_.erase(it); return std::nullopt; }
    return it->second.value;
}

bool Store::del(const std::string& key, Clock::time_point now) {
    auto it = kv_.find(key);
    if (it == kv_.end() || expired(it->second, now)) {
        kv_.erase(key);  // also drop expired shadow
        return false;
    }
    kv_.erase(it);
    return true;
}

std::optional<long long> Store::incr(const std::string& key, Clock::time_point now) {
    auto it = kv_.find(key);
    long long v = 0;
    if (it != kv_.end()) {
        if (expired(it->second, now)) {
            kv_.erase(it);
        } else {
            char* end = nullptr;
            long long parsed = std::strtoll(it->second.value.c_str(), &end, 10);
            if (end == it->second.value.c_str() || *end != '\0')
                return std::nullopt;  // not an integer
            v = parsed;
            kv_.erase(it);
        }
    }
    v += 1;
    Entry e;
    e.value = std::to_string(v);
    kv_[key] = std::move(e);
    return v;
}

bool Store::expire(const std::string& key, std::chrono::seconds ttl,
                   Clock::time_point now) {
    auto it = kv_.find(key);
    if (it == kv_.end() || expired(it->second, now)) {
        kv_.erase(key);
        return false;
    }
    if (ttl.count() <= 0) {
        kv_.erase(it);
        return true;
    }
    it->second.expiresAt = now + ttl;
    return true;
}

long long Store::ttlSeconds(const std::string& key, Clock::time_point now) const {
    auto it = kv_.find(key);
    if (it == kv_.end() || expired(it->second, now)) return -2;
    if (!it->second.expiresAt) return -1;
    auto remain = std::chrono::duration_cast<std::chrono::seconds>(
        *it->second.expiresAt - now);
    long long secs = remain.count();
    // round up sub-second remainders (Redis behaves this way)
    if (*it->second.expiresAt > now && secs * 1000000000LL == 0) secs = 1;
    if (*it->second.expiresAt - now > remain) ++secs;
    return secs < 0 ? 0 : secs;
}

std::size_t Store::dbSize(Clock::time_point now) const {
    std::size_t n = 0;
    for (const auto& [k, e] : kv_)
        if (!expired(e, now)) ++n;
    return n;
}

void Store::purge(Clock::time_point now) {
    for (auto it = kv_.begin(); it != kv_.end();) {
        if (expired(it->second, now)) it = kv_.erase(it);
        else ++it;
    }
}

void Store::maybePurge(Clock::time_point now) {
    if (now - lastPurge_ >= tick_) {
        purge(now);
        lastPurge_ = now;
    }
}

std::string Store::execute(const std::string& commandLine) {
    auto now = Clock::now();
    auto args = splitArgs(commandLine);
    if (args.empty()) return error("empty command");

    std::string cmd = args[0];
    for (auto& c : cmd) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    if (cmd == "SET") {
        if (args.size() < 3) return error("wrong number of arguments for 'SET'");
        std::optional<std::chrono::seconds> ttl;
        if (args.size() >= 5) {
            std::string flag = args[3];
            for (auto& c : flag) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            if (flag != "EX") return error("syntax error");
            char* end = nullptr;
            long secs = std::strtol(args[4].c_str(), &end, 10);
            if (end == args[4].c_str() || *end != '\0' || secs <= 0)
                return error("invalid expire time");
            ttl = std::chrono::seconds(secs);
        }
        set(args[1], args[2], ttl, now);
        return "+OK\r\n";
    }
    if (cmd == "GET") {
        if (args.size() != 2) return error("wrong number of arguments for 'GET'");
        auto v = get(args[1], now);
        return v ? bulk(*v) : nullBulk();
    }
    if (cmd == "DEL") {
        if (args.size() != 2) return error("wrong number of arguments for 'DEL'");
        return integer(del(args[1], now) ? 1 : 0);
    }
    if (cmd == "INCR") {
        if (args.size() != 2) return error("wrong number of arguments for 'INCR'");
        auto v = incr(args[1], now);
        if (!v) return error("value is not an integer or out of range");
        return integer(*v);
    }
    if (cmd == "EXPIRE") {
        if (args.size() != 3) return error("wrong number of arguments for 'EXPIRE'");
        char* end = nullptr;
        long secs = std::strtol(args[2].c_str(), &end, 10);
        if (end == args[2].c_str() || *end != '\0')
            return error("value is not an integer or out of range");
        return integer(expire(args[1], std::chrono::seconds(secs), now) ? 1 : 0);
    }
    if (cmd == "TTL") {
        if (args.size() != 2) return error("wrong number of arguments for 'TTL'");
        return integer(ttlSeconds(args[1], now));
    }
    if (cmd == "DBSIZE") {
        if (args.size() != 1) return error("wrong number of arguments for 'DBSIZE'");
        return integer(static_cast<long long>(dbSize(now)));
    }
    return error("unknown command '" + args[0] + "'");
}

}  // namespace mini_redis
