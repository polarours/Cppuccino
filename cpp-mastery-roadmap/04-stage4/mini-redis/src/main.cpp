#include "mini_redis.hpp"

#include <iostream>

int main() {
    mini_redis::Store store;

    const char* script[] = {
        "SET name cppuccino",
        "GET name",
        "SET temp value EX 100",
        "TTL temp",
        "INCR counter",
        "INCR counter",
        "INCR counter",
        "DBSIZE",
        "DEL temp",
        "GET temp",
        "BADCMD x",
    };
    for (auto* line : script) {
        std::cout << "> " << line << "\n";
        std::cout << "  " << store.execute(line);
    }
    return 0;
}
