#include "lfu_cache.hpp"

#include <iostream>

int main() {
    lfu_cache::LFUCache<std::string, int> cache(2);
    cache.put("a", 1);
    cache.put("b", 2);
    cache.get("a");          // a becomes more frequent
    cache.put("c", 3);       // evicts b (freq 1, LRU among them)

    std::cout << "a: " << (cache.get("a") ? "hit" : "miss") << "\n";
    std::cout << "b: " << (cache.get("b") ? "hit" : "miss") << "\n";
    std::cout << "c: " << (cache.get("c") ? "hit" : "miss") << "\n";
    return 0;
}
