#include "mini_redis.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

using mini_redis::Clock;
using namespace std::chrono;

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_set_get_del() {
    mini_redis::Store s;
    auto now = Clock::now();
    expect(s.set("k", "v", std::nullopt, now), "set");
    expect(s.get("k", now).value_or("") == "v", "get");
    expect(s.del("k", now), "del existing");
    expect(!s.get("k", now).has_value(), "gone");
    expect(!s.del("k", now), "del missing -> false");
}

void test_expiry_on_access() {
    mini_redis::Store s;
    auto now = Clock::now();
    s.set("k", "v", std::chrono::seconds(1), now);
    expect(s.get("k", now + milliseconds(999)).has_value(), "alive before 1s");
    expect(!s.get("k", now + seconds(1)).has_value(), "expired at 1s");
    expect(s.dbSize(now + seconds(1)) == 0, "expired key not counted");
}

void test_ttl_semantics() {
    mini_redis::Store s;
    auto now = Clock::now();
    s.set("noTtl", "v", std::nullopt, now);
    s.set("withTtl", "v", std::chrono::seconds(30), now);
    expect(s.ttlSeconds("missing", now) == -2, "-2 missing");
    expect(s.ttlSeconds("noTtl", now) == -1, "-1 no ttl");
    long t = s.ttlSeconds("withTtl", now + seconds(10));
    expect(t == 20, "20 remaining (got " + std::to_string(t) + ")");
    expect(s.ttlSeconds("withTtl", now + seconds(31)) == -2, "expired -> -2");
}

void test_incr() {
    mini_redis::Store s;
    auto now = Clock::now();
    expect(s.incr("n", now).value_or(-1) == 1, "missing starts at 1");
    expect(s.incr("n", now).value_or(-1) == 2, "increment");
    s.set("n", "41", std::nullopt, now);
    expect(s.incr("n", now).value_or(-1) == 42, "from existing value");
    s.set("bad", "abc", std::nullopt, now);
    expect(!s.incr("bad", now).has_value(), "non-integer rejected");
}

void test_expire_command() {
    mini_redis::Store s;
    auto now = Clock::now();
    s.set("k", "v", std::nullopt, now);
    expect(s.expire("k", seconds(1), now), "expire existing");
    expect(s.ttlSeconds("k", now) > 0, "ttl now positive");
    expect(!s.expire("missing", seconds(10), now), "expire missing false");
}

void test_purge_removes_expired() {
    mini_redis::Store s;
    auto now = Clock::now();
    s.set("a", "1", seconds(1), now);
    s.set("b", "2", std::nullopt, now);
    s.purge(now + seconds(2));
    expect(s.dbSize(now + seconds(2)) == 1, "only live key remains");
}

void test_execute_protocol_responses() {
    mini_redis::Store s;
    expect(s.execute("SET a hello") == "+OK\r\n", "SET -> +OK");
    expect(s.execute("GET a") == "$5\r\nhello\r\n", "GET -> bulk");
    expect(s.execute("GET missing") == "$-1\r\n", "missing -> null bulk");
    expect(s.execute("INCR c") == ":1\r\n", "INCR -> :1");
    expect(s.execute("DEL a") == ":1\r\n", "DEL -> :1");
    expect(s.execute("TTL a") == ":−2\r\n" || s.execute("TTL a") == ":-2\r\n",
           "TTL missing -> :-2");
    // "a" was deleted, but INCR created "c" earlier -> one live key
    expect(s.execute("DBSIZE") == ":1\r\n", "DBSIZE counts live keys");
    expect(s.execute("NOPE") == "-ERR unknown command 'NOPE'\r\n", "unknown cmd");
    expect(s.execute("SET x") == "-ERR wrong number of arguments for 'SET'\r\n",
           "arity error");
    expect(s.execute("") == "-ERR empty command\r\n", "empty line");
}

void test_execute_with_ttl_flag() {
    mini_redis::Store s;
    expect(s.execute("SET sess token EX 60") == "+OK\r\n", "SET EX ok");
    auto v = s.execute("GET sess");
    expect(v == "$5\r\ntoken\r\n", "value readable");
    expect(s.execute("SET bad t EX 0") == "-ERR invalid expire time\r\n",
           "EX 0 rejected");
}

void test_split_args() {
    auto args = mini_redis::splitArgs("  SET   k  v  ");
    expect(args.size() == 3, "split + trim");
    expect(args[0] == "SET" && args[2] == "v", "tokens");
}

}  // namespace

int main() {
    try {
        test_set_get_del();
        test_expiry_on_access();
        test_ttl_semantics();
        test_incr();
        test_expire_command();
        test_purge_removes_expired();
        test_execute_protocol_responses();
        test_execute_with_ttl_flag();
        test_split_args();
        std::cout << "All mini_redis tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
